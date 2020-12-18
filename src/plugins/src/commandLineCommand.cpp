#include "commandLineCommand.h"

#include <string>

#include <gsl/string_span>

#include "config/environment.h"
#include "config/variablesMap.h"
#include "core/patterns.h"
#include "core/task.h"
#include "plugins/workingDirectory.h"

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
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsValues;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;

namespace {
const czstring<> PLUGIN_NAME = "command-line-command";
} // namespace

namespace execHelper::plugins {
auto CommandLineCommand::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept
    -> VariablesMap {
    VariablesMap defaults(PLUGIN_NAME);
    if(!defaults.add(COMMAND_LINE_KEY, CommandLineArgs())) {
        LOG(error) << "Failed to add key '" << COMMAND_LINE_KEY << "'";
    }
    if(!defaults.add(ENVIRONMENT_KEY, EnvArgs())) {
        LOG(error) << "Failed to add key '" << ENVIRONMENT_KEY << "'";
    }
    return defaults;
}

auto CommandLineCommand::apply(Task task, const VariablesMap& variables) const
    -> Tasks {
    task.appendToEnvironment(getEnvironment(variables));

    auto workingDir = variables.get<WorkingDir>(WORKING_DIR_KEY);
    if(workingDir) {
        task.setWorkingDirectory(*(workingDir));
    }

    auto commandLine = *(variables.get<CommandLineArgs>(COMMAND_LINE_KEY));
    if(commandLine.empty()) {
        LOG(error) << "Could not find the '" << COMMAND_LINE_KEY
                   << "' setting in the '" << PLUGIN_NAME << "' settings";
        throw std::runtime_error(
            string("Command-line-command plugin: Could not find the '")
                .append(COMMAND_LINE_KEY)
                .append("' setting in the '")
                .append(PLUGIN_NAME)
                .append("' settings"));
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
        for(const auto& commandKey :
            variables.get<SettingsValues>(COMMAND_LINE_KEY, SettingsValues())) {
            SettingsKeys tmpKey = keys;
            tmpKey.emplace_back(commandKey);
            Task newTask = task;
            newTask.append(move(*(variables.get<CommandLineArgs>(tmpKey))));
            tasks.emplace_back(newTask);
        }
    }
    return tasks;
}

auto CommandLineCommand::summary() const noexcept -> std::string {
    return "Command-line-command (internal)";
}
} // namespace execHelper::plugins
