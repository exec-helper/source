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

namespace execHelper {
namespace plugins {
std::string Valgrind::getPluginName() const noexcept { return PLUGIN_NAME; }

VariablesMap
Valgrind::getVariablesMap(const FleetingOptionsInterface& fleetingOptions) const
    noexcept {
    VariablesMap defaults(PLUGIN_NAME);
    defaults.add(COMMAND_LINE_KEY, CommandLineArgs());
    const auto verbosity = fleetingOptions.getVerbosity() ? "yes" : "no";
    defaults.add(VERBOSITY_KEY, verbosity);
    return defaults;
}

bool Valgrind::apply(Task task, const VariablesMap& variables,
                     const Patterns& patterns) const noexcept {
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
        task.append(string("--tool=").append(tool.value()));
    }

    if(variables.get<Verbosity>(VERBOSITY_KEY).value()) {
        task.append("--verbose");
    }

    ensures(variables.get<CommandLineArgs>(COMMAND_LINE_KEY) != std::nullopt);
    task.append(variables.get<CommandLineArgs>(COMMAND_LINE_KEY).value());

    for(const auto& combination : makePatternPermutator(patterns)) {
        Task newTask = replacePatternCombinations(task, combination);
        ExecutePlugin buildExecutePlugin(runCommand.value());
        if(!buildExecutePlugin.apply(newTask, variables, patterns)) {
            return false;
        }
    }
    return true;
}
} // namespace plugins
} // namespace execHelper
