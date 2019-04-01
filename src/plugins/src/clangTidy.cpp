#include "clangTidy.h"

#include <gsl/string_span>

#include "config/pattern.h"
#include "config/variablesMap.h"
#include "log/assertions.h"

#include "commandLine.h"
#include "logger.h"
#include "pluginUtils.h"

using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

namespace {
const czstring<> PLUGIN_NAME = "clang-tidy";
using Sources = vector<string>;
const czstring<> SOURCES_KEY = "sources";
const czstring<> CHECKS_KEY = "checks";
const czstring<> WARNING_AS_ERROR_KEY = "warning-as-errors";
} // namespace

namespace execHelper::plugins {
string ClangTidy::getPluginName() const noexcept { return "clang-tidy"; }

VariablesMap ClangTidy::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept {
    VariablesMap defaults(PLUGIN_NAME);
    defaults.add(COMMAND_LINE_KEY, CommandLineArgs());
    defaults.add(SOURCES_KEY, "*.cpp");
    return defaults;
}

bool ClangTidy::apply(Task task, const VariablesMap& variables,
                      const Patterns& patterns) const noexcept {
    task.append(PLUGIN_NAME);

    auto checks = variables.get<Checks>(CHECKS_KEY);
    if(checks) {
        task.append(getChecks(checks.value()));
    }

    auto warningAsError = variables.get<WarningAsError>(WARNING_AS_ERROR_KEY);
    if(warningAsError) {
        task.append(getWarningAsError(warningAsError.value(),
                                      variables.get<Checks>(CHECKS_KEY, {})));
    }

    task.append(variables.get<CommandLineArgs>(COMMAND_LINE_KEY).value());

    ensures(variables.get<Sources>(SOURCES_KEY) != std::nullopt);
    task.append(variables.get<Sources>(SOURCES_KEY).value());

    for(const auto& combination : makePatternPermutator(patterns)) {
        Task newTask = replacePatternCombinations(task, combination);
        if(!registerTask(newTask)) {
            return false;
        }
    }
    return true;
}

TaskCollection ClangTidy::getChecks(const Checks& checks) noexcept {
    if(checks.empty()) {
        return TaskCollection();
    }
    string result("-checks=");
    result += listChecks(checks);
    return TaskCollection({result});
}

TaskCollection
ClangTidy::getWarningAsError(const WarningAsError& warningAsError,
                             const Checks& checks) noexcept {
    // Check if we are in the special case where we inherit the values from
    // checksCollection
    string listedChecks;
    if(warningAsError.size() == 1U && warningAsError.front() == "all") {
        listedChecks = listChecks(checks);
    } else {
        listedChecks = listChecks(warningAsError);
    }
    if(listedChecks.empty()) {
        return TaskCollection();
    }
    return TaskCollection({"-warnings-as-errors=" + listedChecks});
}

string ClangTidy::listChecks(const Checks& checks) noexcept {
    string result;
    if(checks.empty()) {
        return "";
    }
    for(size_t i = 0; i < checks.size() - 1; ++i) {
        result += checks[i] + ",";
    }
    result += checks.back();
    return result;
}
} // namespace execHelper::plugins
