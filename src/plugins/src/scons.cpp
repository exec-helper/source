#include "scons.h"

#include <string>

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"
#include "core/patterns.h"
#include "log/assertions.h"

#include "commandLine.h"
#include "logger.h"
#include "pluginUtils.h"
#include "threadedness.h"
#include "verbosity.h"

using std::string;
using std::to_string;

using gsl::czstring;

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
const czstring<> PLUGIN_NAME = "scons";
} // namespace

namespace execHelper::plugins {
auto Scons::getPluginName() const noexcept -> string { return "scons"; }

auto Scons::getVariablesMap(const FleetingOptionsInterface& fleetingOptions)
    const noexcept -> VariablesMap {
    VariablesMap defaults(PLUGIN_NAME);
    if(!defaults.add(getBuildDirKey(), ".")) {
        LOG(error) << "Failed to add key '" << getBuildDirKey() << "'";
    }
    if(!defaults.add(COMMAND_LINE_KEY, CommandLineArgs())) {
        LOG(error) << "Failed to add key '" << COMMAND_LINE_KEY << "'";
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

auto Scons::apply(Task task, const VariablesMap& variables,
                  const Patterns& patterns) const noexcept -> bool {
    task.append(PLUGIN_NAME);

    ensures(variables.get<Verbosity>(VERBOSITY_KEY) != std::nullopt);
    if(*(variables.get<Verbosity>(VERBOSITY_KEY))) {
        task.append("--debug=explain");
    }

    ensures(variables.get<Jobs>(JOBS_KEY) != std::nullopt);
    task.append({"--jobs", to_string(*(variables.get<Jobs>(JOBS_KEY)))});

    ensures(variables.get<CommandLineArgs>(COMMAND_LINE_KEY) != std::nullopt);
    task.append(*(variables.get<CommandLineArgs>(COMMAND_LINE_KEY)));

    for(const auto& combination : makePatternPermutator(patterns)) {
        Task newTask = replacePatternCombinations(task, combination);
        if(!registerTask(newTask)) {
            return false;
        }
    }
    return true;
}
} // namespace execHelper::plugins
