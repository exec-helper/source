#include "executePlugin.h"

#include <algorithm>
#include <memory>

#include <gsl/gsl_assert>
#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "log/assertions.h"

#include "commandLineCommand.h"
#include "logger.h"
#include "memory.h"
#include "pluginUtils.h"

using std::find;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

using gsl::czstring;
using gsl::not_null;

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

vector<gsl::not_null< // NOLINT(fuchsia-statically-constructed-objects)
    const FleetingOptionsInterface*>>
    execHelper::plugins::ExecutePlugin::
        m_fleeting; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
vector<SettingsNode> // NOLINT(fuchsia-statically-constructed-objects)
    execHelper::plugins::ExecutePlugin::
        m_settings; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
vector<PatternsHandler> // NOLINT(fuchsia-statically-constructed-objects)
    execHelper::plugins::ExecutePlugin::
        m_patterns; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
vector<execHelper::plugins:: // NOLINT(fuchsia-statically-constructed-objects)
       Plugins>
    execHelper::plugins::ExecutePlugin::
        m_plugins; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

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

auto ExecutePlugin::apply(Task task, const VariablesMap& /*variables*/) const
    -> Tasks {
    if(m_commands.empty()) {
        user_feedback_error("No commands configured to execute");
        LOG(warning) << "No commands configured to execute";
    }

    Tasks tasks;
    auto initialCommand = m_initialCommands.begin();
    for(auto command = m_commands.begin(); command != m_commands.end();
        ++command, ++initialCommand) {
        auto plugin = getNextStep(*command, *initialCommand);
        if(!plugin) {
            LOG(error) << "Could not find a command or plugin called '"
                       << *command << "'";
            throw std::runtime_error(
                string("Could not find a command or plugin called '")
                    .append(*command)
                    .append("'"));
        }

        expects(!m_fleeting.empty());
        expects(!m_settings.empty());
        VariablesMap newVariablesMap =
            plugin->getVariablesMap(*(m_fleeting.back()));
        const vector<SettingsKeys> keys(
            {{*command}, {*command, *initialCommand}});
        getVariablesMap(&newVariablesMap, keys, m_settings.back());

        Task preparedTask = task;
        auto newPatterns = getNextPatterns(newVariablesMap, activePattern());
        preparedTask.addPatterns(newPatterns);
        auto newTasks = plugin->apply(preparedTask, newVariablesMap);
        tasks.insert(tasks.end(), newTasks.begin(), newTasks.end());
    }
    return tasks;
}

auto ExecutePlugin::summary() const noexcept -> std::string {
    return "ExecutePlugin";
}

auto ExecutePlugin::getNextStep(const Command& command,
                                const Command& /*originalCommand*/) noexcept
    -> shared_ptr<const Plugin> {
    const auto pluginNames = getPluginNames();

    if(find(pluginNames.begin(), pluginNames.end(), command) !=
       pluginNames.end()) {
        LOG(trace) << "Retrieving plugin named '" << command << "'.";
        try {
            return getPlugin(command);
        } catch(const InvalidPlugin& /*e*/) {
            LOG(error) << "Unable to retrieve a plugin name '" << command
                       << "'.";
            return nullptr;
        }
    }

    LOG(trace) << "Checking whether '" << command << "' is a known command";
    SettingsNode& settings = m_settings.back();
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

auto ExecutePlugin::getPluginNames() noexcept -> std::vector<std::string> {
    Expects(!m_plugins.empty());
    std::vector<std::string> plugins{"command-line-command", "memory",
                                     "valgrind", "pmd", "lcov"};
    transform(m_plugins.back().begin(), m_plugins.back().end(),
              back_inserter(plugins),
              [](const auto& plugin) { return plugin.first; });
    return plugins;
}

auto ExecutePlugin::getPlugin(const string& pluginName)
    -> shared_ptr<const Plugin> {
    // But, since calling a plugin should not change the internal state of a plugin (see the constness), we can take a shortcut
    // by returning the plugin directly
    if(!m_plugins.empty() && m_plugins.back().count(pluginName) > 0) {
        return m_plugins.back()[pluginName];
    }

    if(pluginName == "command-line-command") {
        return make_shared<CommandLineCommand>();
    }
    if(pluginName == "memory") {
        return make_shared<Memory>();
    }

    throw InvalidPlugin(string("Plugin associated with name '")
                            .append(pluginName)
                            .append("' not found"));
}

auto ExecutePlugin::push(
    not_null<const config::FleetingOptionsInterface*> fleetingOptions) noexcept
    -> shared_ptr<Raii> {
    m_fleeting.push_back(fleetingOptions);
    return shared_ptr<Raii>(new Raii(), []([[maybe_unused]] Raii* raii) {
        ExecutePlugin::popFleetingOptions();
        delete raii; // NOLINT(cppcoreguidelines-owning-memory)
    });
}

auto ExecutePlugin::push(config::SettingsNode&& settings) noexcept
    -> shared_ptr<Raii> {
    m_settings.emplace_back(settings);
    return shared_ptr<Raii>(new Raii(), []([[maybe_unused]] Raii* raii) {
        ExecutePlugin::popSettingsNode();
        delete raii; // NOLINT(cppcoreguidelines-owning-memory)
    });
}

auto ExecutePlugin::push(config::Patterns&& patterns) noexcept
    -> shared_ptr<Raii> {
    m_patterns.emplace_back(patterns);
    return shared_ptr<Raii>(new Raii(), []([[maybe_unused]] Raii* raii) {
        ExecutePlugin::popPatterns();
        delete raii; // NOLINT(cppcoreguidelines-owning-memory)
    });
}

auto ExecutePlugin::push(Plugins&& plugins) noexcept -> shared_ptr<Raii> {
    m_plugins.emplace_back(plugins);
    return shared_ptr<Raii>(new Raii(), []([[maybe_unused]] Raii* raii) {
        ExecutePlugin::popPlugins();
        delete raii; // NOLINT(cppcoreguidelines-owning-memory)
    });
}

auto ExecutePlugin::activePattern() noexcept -> const PatternsHandler& {
    expects(!m_patterns.empty());
    return m_patterns.back();
}

void ExecutePlugin::popFleetingOptions() noexcept {
    Expects(!m_fleeting.empty());
    m_fleeting.pop_back();
}

void ExecutePlugin::popSettingsNode() noexcept {
    Expects(!m_settings.empty());
    m_settings.pop_back();
}

void ExecutePlugin::popPatterns() noexcept {
    Expects(!m_patterns.empty());
    m_patterns.pop_back();
}

void ExecutePlugin::popPlugins() noexcept {
    Expects(!m_plugins.empty());
    m_plugins.pop_back();
}
} // namespace execHelper::plugins
