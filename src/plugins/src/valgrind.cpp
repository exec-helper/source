#include "valgrind.h"

#include <string>

#include <gsl/string_span>

#include "config/commandLineOptions.h"
#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"
#include "core/patterns.h"
#include "core/task.h"
#include "log/assertions.h"

#include "commandLine.h"
#include "executePlugin.h"
#include "logger.h"
#include "pluginUtils.h"
#include "verbosity.h"

using std::string;

using gsl::czstring;

using execHelper::config::CommandCollection;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
const czstring<> PLUGIN_NAME = "valgrind";
using RunCommand = CommandCollection;
const czstring<> RUN_COMMAND_KEY = "run-command";
using Tool = string;
const czstring<> TOOL_KEY = "tool";
} // namespace

namespace execHelper::plugins {
auto Valgrind::getPluginName() const noexcept -> string { return PLUGIN_NAME; }

auto Valgrind::getVariablesMap(const FleetingOptionsInterface& fleetingOptions)
    const noexcept -> VariablesMap {
    VariablesMap defaults(PLUGIN_NAME);
    if(!defaults.add(COMMAND_LINE_KEY, CommandLineArgs())) {
        LOG(error) << "Failed to add key '" << COMMAND_LINE_KEY << "'";
    }
    const auto verbosity = fleetingOptions.getVerbosity() ? "yes" : "no";
    if(!defaults.add(VERBOSITY_KEY, verbosity)) {
        LOG(error) << "Failed to add key '" << VERBOSITY_KEY << "'";
    }
    return defaults;
}

auto Valgrind::apply(Task task, const VariablesMap& variables,
                     const Patterns& patterns) const noexcept -> bool {
    task.append(PLUGIN_NAME);

    auto runCommand = variables.get<RunCommand>(RUN_COMMAND_KEY);
    if(runCommand == std::nullopt) {
        user_feedback_error("Could not find the '"
                            << RUN_COMMAND_KEY << "' setting in the '"
                            << PLUGIN_NAME << "' settings");
        return false;
    }

    auto tool = variables.get<Tool>(TOOL_KEY);
    if(tool) {
        task.append(string("--tool=").append(*tool));
    }

    if(*(variables.get<Verbosity>(VERBOSITY_KEY))) {
        task.append("--verbose");
    }

    ensures(variables.get<CommandLineArgs>(COMMAND_LINE_KEY) != std::nullopt);
    task.append(*(variables.get<CommandLineArgs>(COMMAND_LINE_KEY)));

    for(const auto& combination : makePatternPermutator(patterns)) {
        Task newTask = replacePatternCombinations(task, combination);
        ExecutePlugin buildExecutePlugin(*runCommand);
        if(!buildExecutePlugin.apply(newTask, variables, patterns)) {
            return false;
        }
    }
    return true;
}
} // namespace execHelper::plugins
