#include "executePlugin.h"

#include <algorithm>
#include <memory>

#include <gsl/gsl_assert>
#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "log/assertions.h"

#include "commandLineCommand.h"
#include "executionContext.h"
#include "logger.h"
#include "memory.h"
#include "pluginUtils.h"

using std::find;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::CommandCollection;
using execHelper::config::FleetingOptionsInterface;
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
auto getNextPatterns(const VariablesMap& variables,
                     const PatternsHandler& patterns) -> Patterns {
    auto newPatternKeys =
        variables.get<PatternKeys>(execHelper::plugins::getPatternsKey(), {});
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
} // namespace

namespace execHelper::plugins {
ExecutePlugin::ExecutePlugin(
    const CommandCollection& commandsToExecute) noexcept
    : m_commands(commandsToExecute), m_initialCommands(commandsToExecute) {
    ensures(m_commands.size() == m_initialCommands.size());
}

ExecutePlugin::ExecutePlugin(const CommandCollection& commandsToExecute,
                             const Command& initialCommand) noexcept
    : m_commands(commandsToExecute),
      m_initialCommands(commandsToExecute.size(), initialCommand) {
    ensures(m_commands.size() == m_initialCommands.size());
}

auto ExecutePlugin::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept
    -> VariablesMap {
    return VariablesMap("execute-plugin");
}

inline void ExecutePlugin::index(VariablesMap* variables,
                                 const SettingsNode& settings,
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

inline auto ExecutePlugin::getVariablesMap(
    VariablesMap* variables, const vector<SettingsKeys>& keys,
    const SettingsNode& rootSettings) noexcept -> bool {
    for(const auto& key : keys) {
        if(rootSettings.contains(key)) {
            index(variables, rootSettings, key);
        }
    }
    return true;
}

auto ExecutePlugin::apply(Task task, const VariablesMap& /*variables*/,
                          const ExecutionContext& context) const -> Tasks {
    if(m_commands.empty()) {
        user_feedback_error("No commands configured to execute");
        LOG(warning) << "No commands configured to execute";
    }

    Tasks tasks;
    auto initialCommand = m_initialCommands.begin();
    for(auto command = m_commands.begin(); command != m_commands.end();
        ++command, ++initialCommand) {
        auto plugin = getNextStep(*command, *initialCommand, context.settings(),
                                  context.plugins());
        if(!plugin) {
            LOG(error) << "Could not find a command or plugin called '"
                       << *command << "'";
            throw std::runtime_error(
                string("Could not find a command or plugin called '")
                    .append(*command)
                    .append("'"));
        }

        VariablesMap newVariablesMap =
            plugin->getVariablesMap(context.options());
        const vector<SettingsKeys> keys(
            {{*command}, {*command, *initialCommand}});
        getVariablesMap(&newVariablesMap, keys, context.settings());

        Task preparedTask = task;
        auto newPatterns = getNextPatterns(newVariablesMap, context.patterns());
        preparedTask.addPatterns(newPatterns);
        auto newTasks = plugin->apply(preparedTask, newVariablesMap, context);
        tasks.insert(tasks.end(), newTasks.begin(), newTasks.end());
    }
    return tasks;
}

auto ExecutePlugin::summary() const noexcept -> std::string {
    return "ExecutePlugin";
}

auto ExecutePlugin::getNextStep(const Command& command,
                                const Command& /*originalCommand*/,
                                const SettingsNode& settings,
                                const Plugins& plugins) noexcept
    -> shared_ptr<const Plugin> {
    const auto pluginNames = getPluginNames(plugins);

    if(find(pluginNames.begin(), pluginNames.end(), command) !=
       pluginNames.end()) {
        LOG(trace) << "Retrieving plugin named '" << command << "'.";
        try {
            return plugins.at(command);
        } catch(const InvalidPlugin& /*e*/) {
            LOG(error) << "Unable to retrieve a plugin name '" << command
                       << "'.";
            return nullptr;
        }
    }

    LOG(trace) << "Checking whether '" << command << "' is a known command";
    auto commandToExecuteOpt = settings.get<CommandCollection>(command);
    if(!commandToExecuteOpt) {
        LOG(warning)
            << "Execute plugin found an empty value collection for the '"
            << command << "' command";
        return nullptr;
    }
    auto commandsToExecute = *(commandToExecuteOpt);
    for(const auto& commandToExecute : commandsToExecute) {
        LOG(trace) << command << " -> " << commandToExecute;
    }
    return make_shared<ExecutePlugin>(commandsToExecute, command);
}

inline auto ExecutePlugin::getPluginNames(const Plugins& plugins) noexcept
    -> std::vector<std::string> {
    vector<string> names;
    names.reserve(plugins.size());

    transform(plugins.begin(), plugins.end(), back_inserter(names),
              [](const auto& plugin) { return plugin.first; });
    return names;
}
} // namespace execHelper::plugins
