#include "commandLineCommand.h"

#include <string>

#include <gsl/string_span>

#include "config/environment.h"
#include "config/variablesMap.h"
#include "config/workingDirectory.h"
#include "core/patterns.h"
#include "core/task.h"

#include "commandLine.h"
#include "logger.h"
#include "pluginUtils.h"

using std::move;
using std::string;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::EnvArgs;
using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::EnvironmentCollection;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Path;
using execHelper::config::Patterns;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsValues;
using execHelper::config::VariablesMap;
using execHelper::config::WORKING_DIR_KEY;
using execHelper::config::WorkingDir;
using execHelper::core::Task;
using execHelper::core::Tasks;

namespace {
const czstring<> PLUGIN_NAME = "command-line-command";
} // namespace

namespace execHelper {
namespace plugins {

std::string CommandLineCommand::getPluginName() const noexcept {
    return PLUGIN_NAME;
}

config::VariablesMap CommandLineCommand::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept {
    VariablesMap defaults(PLUGIN_NAME);
    defaults.add(COMMAND_LINE_KEY, CommandLineArgs());
    defaults.add(ENVIRONMENT_KEY, EnvArgs());
    return defaults;
}

bool CommandLineCommand::apply(Task task, const VariablesMap& variables,
                               const Patterns& patterns) const noexcept {
    task.appendToEnvironment(getEnvironment(variables));

    auto workingDir = variables.get<WorkingDir>(WORKING_DIR_KEY);
    if(workingDir) {
        task.setWorkingDirectory(*(workingDir));
    }

    auto commandLine = *(variables.get<CommandLineArgs>(COMMAND_LINE_KEY));
    if(commandLine.empty()) {
        user_feedback_error("Could not find the '"
                            << COMMAND_LINE_KEY << "' setting in the '"
                            << PLUGIN_NAME << "' settings");
        return false;
    }

    Tasks tasks;
    if(variables
           .get<SettingsValues>(
               SettingsKeys({COMMAND_LINE_KEY, commandLine.front()}),
               SettingsValues())
           .empty()) {
        task.append(move(commandLine));
        tasks.emplace_back(move(task));
    } else {
        SettingsKeys keys({COMMAND_LINE_KEY});
        for(auto commandKey :
            variables.get<SettingsValues>(COMMAND_LINE_KEY, SettingsValues())) {
            SettingsKeys tmpKey = keys;
            tmpKey.emplace_back(commandKey);
            Task newTask = task;
            newTask.append(move(*(variables.get<CommandLineArgs>(tmpKey))));
            tasks.emplace_back(newTask);
        }
    }

    for(const auto& combination : makePatternPermutator(patterns)) {
        for(const auto& executeTask : tasks) {
            Task newTask = replacePatternCombinations(executeTask, combination);
            if(!registerTask(newTask)) {
                return false;
            }
        }
    }
    return true;
}
} // namespace plugins
} // namespace execHelper
