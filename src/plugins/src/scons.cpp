#include "scons.h"

#include <string>

#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"
#include "core/patterns.h"
#include "log/assertions.h"

#include "commandLine.h"
#include "pluginUtils.h"
#include "threadedness.h"
#include "verbosity.h"

using std::string;
using std::to_string;

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
const std::string PLUGIN_NAME = "scons";
} // namespace

namespace execHelper {
namespace plugins {
std::string Scons::getPluginName() const noexcept { return "scons"; }

VariablesMap
Scons::getVariablesMap(const FleetingOptionsInterface& fleetingOptions) const
    noexcept {
    VariablesMap defaults(PLUGIN_NAME);
    defaults.add(getBuildDirKey(), ".");
    defaults.add(COMMAND_LINE_KEY, CommandLineArgs());
    const auto verbosity = fleetingOptions.getVerbosity() ? "yes" : "no";
    defaults.add(VERBOSITY_KEY, verbosity);
    defaults.add(JOBS_KEY, to_string(fleetingOptions.getJobs()));
    return defaults;
}

bool Scons::apply(Task task, const VariablesMap& variables,
                  const Patterns& patterns) const noexcept {
    task.append(PLUGIN_NAME);

    ensures(variables.get<Verbosity>(VERBOSITY_KEY) != boost::none);
    if(variables.get<Verbosity>(VERBOSITY_KEY).get()) {
        task.append("--debug=explain");
    }

    ensures(variables.get<Jobs>(JOBS_KEY) != boost::none);
    task.append({"--jobs", to_string(variables.get<Jobs>(JOBS_KEY).get())});

    ensures(variables.get<CommandLineArgs>(COMMAND_LINE_KEY) != boost::none);
    task.append(variables.get<CommandLineArgs>(COMMAND_LINE_KEY).get());

    for(const auto& combination : makePatternPermutator(patterns)) {
        Task newTask = replacePatternCombinations(task, combination);
        if(!registerTask(newTask)) {
            return false;
        }
    }
    return true;
}
} // namespace plugins
} // namespace execHelper
