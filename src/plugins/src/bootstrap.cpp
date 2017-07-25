#include "bootstrap.h"

#include <iostream>
#include <string>

#include <gsl/string_span>

#include "config/path.h"
#include "core/task.h"

#include "commandLine.h"
#include "logger.h"
#include "pluginUtils.h"

using std::string;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Path;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
    const czstring<> PLUGIN_NAME = "bootstrap";
    const czstring<> FILENAME_KEY = "filename";
} // namespace

namespace execHelper { namespace plugins {
    string Bootstrap::getPluginName() const noexcept {
        return PLUGIN_NAME;
    }

    VariablesMap Bootstrap::getVariablesMap(const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept {
        VariablesMap defaults(PLUGIN_NAME);
        defaults.add(getBuildDirKey(), ".");
        defaults.add(FILENAME_KEY, "bootstrap.sh");
        defaults.add(COMMAND_LINE_KEY);
        return defaults;
    }

    bool Bootstrap::apply(Task task, const VariablesMap& variables, const Patterns& patterns) const noexcept {
        auto filename = variables.get<Path>(FILENAME_KEY).get();
        if(filename.is_relative()) {
            filename = Path(".") / filename;
        }
        task.append(filename.native());
        task.append(variables.get<CommandLineArgs>(COMMAND_LINE_KEY).get());
        task.setWorkingDirectory(variables.get<Path>(getBuildDirKey()).get());

        for(const auto& combination : makePatternPermutator(patterns)) {
            Task bootstrapTask = replacePatternCombinations(task, combination);
            registerTask(bootstrapTask);
        }
        return true;
    }
} // namespace plugins
} // namespace execHelper
