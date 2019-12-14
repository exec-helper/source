#include "clangTidy.h"

#include <gsl/string_span>

#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/task.h"
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
auto ClangTidy::getPluginName() const noexcept -> string {
    return "clang-tidy";
}

auto ClangTidy::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept
    -> VariablesMap {
    VariablesMap defaults(PLUGIN_NAME);
    if(!defaults.add(COMMAND_LINE_KEY, CommandLineArgs())) {
        LOG(error) << "Failed to add key '" << COMMAND_LINE_KEY << "'";
    }
    if(!defaults.add(SOURCES_KEY, "*.cpp")) {
        LOG(error) << "Failed to add key '" << SOURCES_KEY << "'";
    }
    return defaults;
}

auto ClangTidy::apply(Task task, const VariablesMap& variables,
                      const Patterns& patterns) const noexcept -> bool {
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

auto ClangTidy::getChecks(const Checks& checks) noexcept -> TaskCollection {
    if(checks.empty()) {
        return TaskCollection();
    }
    string result("-checks=");
    result += listChecks(checks);
    return TaskCollection({result});
}

auto ClangTidy::getWarningAsError(const WarningAsError& warningAsError,
                                  const Checks& checks) noexcept
    -> TaskCollection {
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

auto ClangTidy::listChecks(const Checks& checks) noexcept -> string {
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
