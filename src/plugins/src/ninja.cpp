#include "ninja.h"

#include <string>

#include <gsl/string_span>

#include "config/environment.h"
#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"
#include "config/workingDirectory.h"
#include "core/patterns.h"
#include "core/task.h"

#include "commandLine.h"
#include "pluginUtils.h"
#include "threadedness.h"
#include "verbosity.h"

using std::string;
using std::to_string;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::EnvArgs;
using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Path;
using execHelper::config::PatternKeys;
using execHelper::config::VariablesMap;
using execHelper::config::WORKING_DIR_KEY;
using execHelper::config::WorkingDir;
using execHelper::core::Task;

namespace {
const czstring<> NINJA_KEY = "ninja";
using BuildDir = string;
} // namespace

namespace execHelper {
namespace plugins {
std::string Ninja::getPluginName() const noexcept { return NINJA_KEY; }

VariablesMap
Ninja::getVariablesMap(const FleetingOptionsInterface& fleetingOptions) const
    noexcept {
    VariablesMap defaults(NINJA_KEY);
    defaults.add(getBuildDirKey(), ".");
    defaults.add(COMMAND_LINE_KEY, CommandLineArgs());
    const auto verbosity = fleetingOptions.getVerbosity() ? "yes" : "no";
    defaults.add(VERBOSITY_KEY, verbosity);
    defaults.add(JOBS_KEY, to_string(fleetingOptions.getJobs()));
    defaults.add(ENVIRONMENT_KEY, EnvArgs());
    return defaults;
}

bool Ninja::apply(core::Task task, const config::VariablesMap& variables,
                  const config::Patterns& patterns) const noexcept {
    task.append(NINJA_KEY);
    task.append({"-C", variables.get<Path>(getBuildDirKey())->native()});
    task.append({"-j", to_string(*(variables.get<Jobs>(JOBS_KEY)))});

    auto workingDir = variables.get<WorkingDir>(WORKING_DIR_KEY);
    if(workingDir) {
        task.setWorkingDirectory(*workingDir);
    }

    if(*(variables.get<Verbosity>(VERBOSITY_KEY))) {
        task.append("-v");
    }
    task.append(*(variables.get<CommandLineArgs>(COMMAND_LINE_KEY)));
    task.appendToEnvironment(getEnvironment(variables));

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
