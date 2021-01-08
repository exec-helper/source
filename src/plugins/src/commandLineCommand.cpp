#include "commandLineCommand.h"

#include <string>
#include <utility>

#include "config/environment.h"
#include "config/variablesMap.h"
#include "core/patterns.h"
#include "core/task.h"

#include "commandLine.h"
#include "executionContext.h"
#include "logger.h"
#include "pluginUtils.h"
#include "workingDirectory.h"

using std::move;
using std::string;

using execHelper::config::Path;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsValues;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;

namespace execHelper::plugins {
[[nodiscard]] auto
commandLineCommand(Task task, const VariablesMap& variables,
                   [[maybe_unused]] const ExecutionContext& context) -> Tasks {
    task.appendToEnvironment(getEnvironment(variables));

    auto workingDir = variables.get<WorkingDir>(WORKING_DIR_KEY);
    if(workingDir) {
        task.setWorkingDirectory(*(workingDir));
    }

    auto commandLine = variables.get<CommandLineArgs>(COMMAND_LINE_KEY);
    if(!commandLine) {
        LOG(error) << "Could not find the '" << COMMAND_LINE_KEY
                   << "' setting in the 'command-line-command' settings";
        throw std::runtime_error(
            string("Command-line-command plugin: Could not find the '")
                .append(COMMAND_LINE_KEY)
                .append("' setting in the 'command-line-command' settings"));
    }

    Tasks tasks;
    if(variables
           .get<SettingsValues>(
               SettingsKeys({COMMAND_LINE_KEY, commandLine->front()}),
               SettingsValues())
           .empty()) {
        task.append(move(*commandLine));
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
} // namespace execHelper::plugins
