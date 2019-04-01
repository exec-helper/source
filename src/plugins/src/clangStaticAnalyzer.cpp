#include "clangStaticAnalyzer.h"

#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"
#include "core/patterns.h"
#include "core/task.h"
#include "log/assertions.h"

#include "commandLine.h"
#include "executePlugin.h"
#include "logger.h"
#include "verbosity.h"

using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
const czstring<> PLUGIN_NAME = "clang-static-analyzer";
const czstring<> BIN_NAME = "scan-build";
using BuildCommands = vector<string>;
const czstring<> BUILD_COMMANDS_KEY = "build-command";
} // namespace

namespace execHelper::plugins {

std::string ClangStaticAnalyzer::getPluginName() const noexcept {
    return PLUGIN_NAME;
}

config::VariablesMap ClangStaticAnalyzer::getVariablesMap(
    const config::FleetingOptionsInterface& fleetingOptions) const noexcept {
    VariablesMap defaults(PLUGIN_NAME);
    defaults.add(getBuildDirKey(), ".");
    defaults.add(COMMAND_LINE_KEY, CommandLineArgs());
    const auto verbosity = fleetingOptions.getVerbosity() ? "yes" : "no";
    defaults.add(VERBOSITY_KEY, verbosity);
    return defaults;
}

bool ClangStaticAnalyzer::apply(core::Task task,
                                const config::VariablesMap& variables,
                                const config::Patterns& patterns) const
    noexcept {
    task.append(BIN_NAME);
    auto buildCommand = variables.get<BuildCommands>(BUILD_COMMANDS_KEY);
    if(!buildCommand || buildCommand.value().empty()) {
        user_feedback_error(
            "You must define at least one build command for the "
            << PLUGIN_NAME << " plugin.");
        return false;
    }

    ensures(variables.get<Verbosity>(VERBOSITY_KEY) != std::nullopt);
    if(*(variables.get<Verbosity>(VERBOSITY_KEY))) {
        task.append("-v");
    }
    ensures(variables.get<CommandLineArgs>(COMMAND_LINE_KEY) != std::nullopt);
    task.append(*(variables.get<CommandLineArgs>(COMMAND_LINE_KEY)));

    ExecutePlugin buildExecutePlugin(*(buildCommand));
    return buildExecutePlugin.apply(task, variables, patterns);
}
} // namespace execHelper::plugins
