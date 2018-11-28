#include "executePlugin.h"

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "log/assertions.h"

#include "bootstrap.h"
#include "clangStaticAnalyzer.h"
#include "clangTidy.h"
#include "commandLineCommand.h"
#include "cppcheck.h"
#include "lcov.h"
#include "logger.h"
#include "make.h"
#include "memory.h"
#include "plugin.h"
#include "pluginUtils.h"
#include "pmd.h"
#include "scons.h"
#include "selector.h"
#include "valgrind.h"

using std::make_unique;
using std::string;
using std::unique_ptr;
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

namespace {
const czstring<> EXECUTE_PLUGIN_KEY = "execute-plugin";

Patterns getNextPatterns(const VariablesMap& variables,
                         const PatternsHandler& patterns) {
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
    execHelper::plugins::ExecutePlugin::m_fleeting;
vector<SettingsNode> // NOLINT(fuchsia-statically-constructed-objects)
    execHelper::plugins::ExecutePlugin::m_settings;
vector<PatternsHandler> // NOLINT(fuchsia-statically-constructed-objects)
    execHelper::plugins::ExecutePlugin::m_patterns;

namespace execHelper {
namespace plugins {
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

std::string ExecutePlugin::getPluginName() const noexcept {
    return EXECUTE_PLUGIN_KEY;
}

VariablesMap ExecutePlugin::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept {
    return VariablesMap("execute-plugin");
}

SettingsKey ExecutePlugin::getPatternsKey() const noexcept {
    return "no-patterns-allowed";
}

inline void ExecutePlugin::index(VariablesMap* variables,
                                 const SettingsNode& settings,
                                 const SettingsKeys& key) noexcept {
    if(!settings.contains(key)) {
        return;
    }

    expects(!key.empty());
    variables->replace(key.back(), *(settings.get<SettingsValues>(key)));

    // Get current depth to the level of the given key
    const SettingsNode& currentDepth = std::accumulate(
        key.begin(), key.end(), static_cast<const SettingsNode&>(settings),
        [](const auto& node, const auto& subkey) { return node[subkey]; });

    for(const auto& depthKey :
        currentDepth.get<SettingsValues>(SettingsKeys(), SettingsValues())) {
        (*variables)[depthKey] = currentDepth[depthKey];
    }
}

inline bool
ExecutePlugin::getVariablesMap(VariablesMap* variables,
                               const vector<SettingsKeys>& keys,
                               const SettingsNode& rootSettings) noexcept {
    for(const auto& key : keys) {
        if(rootSettings.contains(key)) {
            index(variables, rootSettings, key);
        }
    }
    return true;
}

bool ExecutePlugin::apply(Task task, const VariablesMap& /*variables*/,
                          const Patterns& /*patterns*/) const noexcept {
    if(m_commands.empty()) {
        user_feedback_error("No commands configured to execute");
        LOG(warning) << "No commands configured to execute";
    }
    auto initialCommand = m_initialCommands.begin();
    for(auto command = m_commands.begin(); command != m_commands.end();
        ++command, ++initialCommand) {
        unique_ptr<Plugin> plugin = getNextStep(*command, *initialCommand);
        if(!plugin) {
            user_feedback_error("Could not find a command or plugin called '"
                                << *command << "'");
            return false;
        }

        expects(!m_fleeting.empty());
        expects(!m_settings.empty());
        VariablesMap newVariablesMap =
            plugin->getVariablesMap(*(m_fleeting.back()));
        const vector<SettingsKeys> keys(
            {{plugin->getPluginName()},
             {plugin->getPluginName(), *initialCommand}});
        getVariablesMap(&newVariablesMap, keys, m_settings.back());

        expects(!m_patterns.empty());
        auto newPatterns = getNextPatterns(newVariablesMap, m_patterns.back());
        if(!plugin->apply(task, newVariablesMap, newPatterns)) {
            user_feedback_error("An error occured executing the '"
                                << *command << "' command");
            return false;
        }
    }
    return true;
}

unique_ptr<Plugin>
ExecutePlugin::getNextStep(const Command& command,
                           const Command& /*originalCommand*/) noexcept {
    unique_ptr<Plugin> plugin = getPlugin(command);
    if(plugin) {
        return plugin;
    }

    // else assume it is another command and use the execute plugin to execute
    // it
    SettingsNode& settings = m_settings.back();
    auto commandToExecuteOpt = settings.get<CommandCollection>(command);
    if(!commandToExecuteOpt) {
        LOG(warning)
            << "Execute plugin found an empty value collection for the '"
            << command << "' command";
        return unique_ptr<Plugin>();
    }
    auto commandsToExecute = *(commandToExecuteOpt);
    for(const auto& commandToExecute : commandsToExecute) {
        LOG(trace) << command << " -> " << commandToExecute;
    }
    return make_unique<ExecutePlugin>(commandsToExecute, command);
}

unique_ptr<Plugin> ExecutePlugin::getPlugin(const string& pluginName) noexcept {
    if(pluginName == "command-line-command") {
        return make_unique<CommandLineCommand>();
    }
    if(pluginName == "scons") {
        return make_unique<Scons>();
    }
    if(pluginName == "make") {
        return make_unique<Make>();
    }
    if(pluginName == "bootstrap") {
        return make_unique<Bootstrap>();
    }
    if(pluginName == "cppcheck") {
        return make_unique<Cppcheck>();
    }
    if(pluginName == "clang-static-analyzer") {
        return make_unique<ClangStaticAnalyzer>();
    }
    if(pluginName == "selector") {
        return make_unique<Selector>();
    }
    if(pluginName == "memory") {
        return make_unique<Memory>();
    }
    if(pluginName == "valgrind") {
        return make_unique<Valgrind>();
    }
    if(pluginName == "pmd") {
        return make_unique<Pmd>();
    }
    if(pluginName == "lcov") {
        return make_unique<Lcov>();
    }
    if(pluginName == "clang-tidy") {
        return make_unique<ClangTidy>();
    }
    return unique_ptr<Plugin>();
}

bool ExecutePlugin::push(not_null<const config::FleetingOptionsInterface*>
                             fleetingOptions) noexcept {
    m_fleeting.push_back(fleetingOptions);
    return true;
}

bool ExecutePlugin::push(config::SettingsNode&& settings) noexcept {
    m_settings.emplace_back(settings);
    return true;
}

bool ExecutePlugin::push(config::Patterns&& patterns) noexcept {
    m_patterns.emplace_back(patterns);
    return true;
}

void ExecutePlugin::popFleetingOptions() noexcept { m_fleeting.pop_back(); }

void ExecutePlugin::popSettingsNode() noexcept { m_settings.pop_back(); }

void ExecutePlugin::popPatterns() noexcept { m_patterns.pop_back(); }
} // namespace plugins
} // namespace execHelper
