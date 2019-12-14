#include "bootstrap.h"

#include <iostream>
#include <string>

#include <gsl/string_span>

#include "config/path.h"
#include "config/variablesMap.h"
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

namespace execHelper::plugins {
auto Bootstrap::getPluginName() const noexcept -> string { return PLUGIN_NAME; }

auto Bootstrap::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept
    -> VariablesMap {
    VariablesMap defaults(PLUGIN_NAME);
    if(!defaults.add(getBuildDirKey(), ".")) {
        LOG(error) << "Failed to add key '" << getBuildDirKey() << "'";
    }
    if(!defaults.add(FILENAME_KEY, "bootstrap.sh")) {
        LOG(error) << "Failed to add key '" << FILENAME_KEY << "'";
    }
    if(!defaults.add(COMMAND_LINE_KEY, CommandLineArgs())) {
        LOG(error) << "Failed to add key '" << COMMAND_LINE_KEY << "'";
    }
    return defaults;
}

auto Bootstrap::apply(Task task, const VariablesMap& variables,
                      const Patterns& patterns) const noexcept -> bool {
    auto filename = *(variables.get<Path>(FILENAME_KEY));
    if(filename.is_relative()) {
        filename = Path(".") / filename;
    }
    task.append(filename.string());
    task.append(*(variables.get<CommandLineArgs>(COMMAND_LINE_KEY)));
    task.setWorkingDirectory(*(variables.get<Path>(getBuildDirKey())));

    for(const auto& combination : makePatternPermutator(patterns)) {
        Task bootstrapTask = replacePatternCombinations(task, combination);
        registerTask(bootstrapTask);
    }
    return true;
}
} // namespace execHelper::plugins
