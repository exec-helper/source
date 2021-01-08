#include "executePlugin.h"

#include <algorithm>
#include <memory>
#include <utility>

#include <gsl/gsl_assert>
#include <stdexcept>

#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "log/assertions.h"

#include "executionContext.h"
#include "logger.h"
#include "plugin.h"

using namespace std;

using execHelper::config::Command;
using execHelper::config::CommandCollection;
using execHelper::config::PatternKeys;
using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsKey;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::SettingsValues;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;

namespace {
using namespace std::literals;
using namespace execHelper::plugins;
constexpr auto patternsKey = "patterns"sv;

auto getNextPatterns(const VariablesMap& variables,
                     const PatternsHandler& patterns) -> Patterns {
    auto newPatternKeys = variables.get<PatternKeys>(string(patternsKey), {});
    Patterns newPatterns;
    for(const auto& key : newPatternKeys) {
        if(!patterns.contains(key)) {
            user_feedback_error("Unknown pattern key '" << key
                                                        << "' is ignored.");
            continue;
        }
        newPatterns.push_back(patterns.getPattern(key));
    }
    return newPatterns;
}

auto getNextStep(const Command& command, const SettingsNode& settings,
                 const Plugins& plugins) -> ApplyFunction {
    auto plugin = plugins.find(command);
    if(plugin != plugins.end()) {
        LOG(trace) << "Identified '" << command << "' as a plugin";
        return plugin->second;
    }

    LOG(trace)
        << "'" << command
        << "' is not a known plugin. Checking whether it is a known command";

    auto commandToExecuteOpt = settings.get<CommandCollection>(
        command); // Check whether the settings node contains a root key that is called <command>
    if(commandToExecuteOpt) {
        auto commandsToExecute = *(commandToExecuteOpt);
        for(const auto& commandToExecute : commandsToExecute) {
            LOG(trace) << command << " -> " << commandToExecute;
        }

        LOG(trace) << "Scheduling subsequent commands";
        return [commandsToExecute,
                command](const Task& task,
                         [[maybe_unused]] const VariablesMap& variables,
                         const ExecutionContext& context) -> Tasks {
            LOG(trace) << "Resolving subsequent commands...";
            Tasks tasks;
            for(const auto& commandToExecute : commandsToExecute) {
                auto newTasks = detail::executeCommands(
                    command, commandToExecute, task, context);
                tasks.insert(tasks.end(), newTasks.begin(), newTasks.end());
            }
            return tasks;
        };
    }

    LOG(error) << "Could not find a command or plugin called '" << command
               << "'";
    throw std::runtime_error(
        string("Could not find a command or plugin called '")
            .append(command)
            .append("'"));
}

inline void index(VariablesMap* variables, const SettingsNode& settings,
                  const SettingsKeys& key) noexcept {
    if(!settings.contains(key)) {
        return;
    }

    expects(!key.empty());
    if(!variables->replace(key.back(), *(settings.get<SettingsValues>(key)))) {
        LOG(error) << "Failed to replace key '" << key.back() << "'";
    }

    // Get current depth to the level of the given key
    const SettingsNode& currentDepth = std::accumulate(
        key.begin(), key.end(), static_cast<const SettingsNode&>(settings),
        [](const auto& node, const auto& subkey) { return node[subkey]; });

    for(const auto& depthKey :
        currentDepth.get<SettingsValues>(SettingsKeys(), SettingsValues())) {
        (*variables)[depthKey] = currentDepth[depthKey];
    }
}

inline auto getVariablesMap(VariablesMap* variables,
                            const vector<SettingsKeys>& keys,
                            const SettingsNode& rootSettings) noexcept -> bool {
    for(const auto& key : keys) {
        if(rootSettings.contains(key)) {
            index(variables, rootSettings, key);
        }
    }
    return true;
}

} // namespace

namespace execHelper::plugins::detail {
auto executeCommands(const config::Command& initialCommand,
                     const config::Command& command, const core::Task& task,
                     const ExecutionContext& context) -> core::Tasks {
    LOG(trace) << "Resolving command '" << command << "'";
    auto applyFunction =
        getNextStep(command, context.settings(), context.plugins());
    VariablesMap newVariablesMap(command);
    const vector<SettingsKeys> keys = {{command}, {command, initialCommand}};
    getVariablesMap(&newVariablesMap, keys, context.settings());

    Task preparedTask = task;
    auto newPatterns = getNextPatterns(newVariablesMap, context.patterns());
    preparedTask.addPatterns(newPatterns);
    return applyFunction(preparedTask, newVariablesMap, context);
}
} // namespace execHelper::plugins::detail

namespace execHelper::plugins {
auto executeCommands(const CommandCollection& commands, const Task& task,
                     const ExecutionContext& context) -> Tasks {
    core::Tasks tasks;
    for(const auto& command : commands) {
        auto newTasks =
            detail::executeCommands(command, command, task, context);
        tasks.insert(tasks.end(), newTasks.begin(), newTasks.end());
    }
    return tasks;
}
} // namespace execHelper::plugins
