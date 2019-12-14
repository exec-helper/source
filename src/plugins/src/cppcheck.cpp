#include "cppcheck.h"

#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/patterns.h"
#include "core/task.h"
#include "log/assertions.h"

#include "commandLine.h"
#include "logger.h"
#include "pluginUtils.h"
#include "threadedness.h"
#include "verbosity.h"

using std::string;
using std::to_string;
using std::vector;

using gsl::czstring;

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::PatternCombinations;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
const czstring<> PLUGIN_NAME = "cppcheck";
using EnableChecks = vector<string>;
const czstring<> ENABLE_CHECKS_KEY = "enable-checks";
using SourceDir = vector<string>;
const czstring<> SRC_DIR_KEY = "src-dir";
} // namespace

namespace execHelper::plugins {

auto Cppcheck::getPluginName() const noexcept -> string { return PLUGIN_NAME; }

auto Cppcheck::getVariablesMap(const FleetingOptionsInterface& fleetingOptions)
    const noexcept -> VariablesMap {
    VariablesMap defaults(PLUGIN_NAME);
    if(!defaults.add(SRC_DIR_KEY, ".")) {
        LOG(error) << "Failed to add key '" << SRC_DIR_KEY << "'";
    }
    if(!defaults.add(COMMAND_LINE_KEY, CommandLineArgs())) {
        LOG(error) << "Failed to add key '" << COMMAND_LINE_KEY << "'";
    }
    if(!defaults.add(ENABLE_CHECKS_KEY, "all")) {
        LOG(error) << "Failed to add key '" << ENABLE_CHECKS_KEY << "'";
    }
    const auto verbosity = fleetingOptions.getVerbosity() ? "yes" : "no";
    if(!defaults.add(VERBOSITY_KEY, verbosity)) {
        LOG(error) << "Failed to add key '" << VERBOSITY_KEY << "'";
    }
    if(!defaults.add(JOBS_KEY, to_string(fleetingOptions.getJobs()))) {
        LOG(error) << "Failed to add key '" << JOBS_KEY << "'";
    }
    return defaults;
}

auto Cppcheck::apply(Task task, const VariablesMap& variables,
                     const Patterns& patterns) const noexcept -> bool {
    task.append(PLUGIN_NAME);

    task.append(getEnabledChecks(variables));

    ensures(variables.get<Verbosity>(VERBOSITY_KEY) != std::nullopt);
    if(*(variables.get<Verbosity>(VERBOSITY_KEY))) {
        task.append("--verbose");
    }

    ensures(variables.get<Jobs>(JOBS_KEY) != std::nullopt);
    task.append({"-j", to_string(*(variables.get<Jobs>(JOBS_KEY)))});

    ensures(variables.get<CommandLineArgs>(COMMAND_LINE_KEY) != std::nullopt);
    task.append(*(variables.get<CommandLineArgs>(COMMAND_LINE_KEY)));

    ensures(variables.get<SourceDir>(SRC_DIR_KEY) != std::nullopt);
    task.append(*(variables.get<SourceDir>(SRC_DIR_KEY)));

    for(const auto& combination : makePatternPermutator(patterns)) {
        Task newTask = replacePatternCombinations(task, combination);
        if(!registerTask(newTask)) {
            return false;
        }
    }
    return true;
}

auto Cppcheck::getEnabledChecks(const VariablesMap& variables) noexcept
    -> string {
    ensures(variables.get<EnableChecks>(ENABLE_CHECKS_KEY) != std::nullopt);
    auto enabledChecks = *(variables.get<EnableChecks>(ENABLE_CHECKS_KEY));

    string result("--enable=");
    for(size_t i = 0; i < enabledChecks.size() - 1; ++i) {
        result += enabledChecks[i] + ',';
    }
    result += enabledChecks.back();
    return result;
}
} // namespace execHelper::plugins
