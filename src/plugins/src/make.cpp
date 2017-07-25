#include "make.h"

#include <string>

#include <gsl/string_span>

#include "config/environment.h"
#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"
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
using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Path;
using execHelper::config::PatternKeys;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
    const czstring<> MAKE_KEY = "make";
    using BuildDir = string;
} // namespace

namespace execHelper { namespace plugins {
    std::string Make::getPluginName() const noexcept {
        return MAKE_KEY;
    }

    VariablesMap Make::getVariablesMap(const FleetingOptionsInterface& fleetingOptions) const noexcept {
        VariablesMap defaults(MAKE_KEY);
        defaults.add(getBuildDirKey(), ".");
        defaults.add(COMMAND_LINE_KEY);
        const auto verbosity = fleetingOptions.getVerbosity() ? "yes" : "no";
        defaults.add(VERBOSITY_KEY, verbosity);
        defaults.add(JOBS_KEY, to_string(fleetingOptions.getJobs()));
        defaults.add(ENVIRONMENT_KEY);
        return defaults;
    }

    bool Make::apply(core::Task task, const config::VariablesMap& variables, const config::Patterns& patterns) const noexcept {
        task.append(MAKE_KEY);
        task.append({"--directory", variables.get<Path>(getBuildDirKey()).get().native()});
        task.append({"--jobs", to_string(variables.get<Jobs>(JOBS_KEY).get())});

        if(variables.get<Verbosity>(VERBOSITY_KEY).get()) {
            task.append("--debug");
        }
        task.append(variables.get<CommandLineArgs>(COMMAND_LINE_KEY).get());
        task.appendToEnvironment(getEnvironment(variables));

        for(const auto& combination : makePatternPermutator(patterns)) {
            Task newTask = replacePatternCombinations(task, combination);
            if(! registerTask(newTask)) {
                return false;
            }
        }
        return true;
    }
} // namespace plugins
} // namespace execHelper
