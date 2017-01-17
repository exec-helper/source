#include "commandLineCommand.h"

#include <string>

#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"

#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;

namespace execHelper { namespace plugins {
    bool CommandLineCommand::apply(const std::string& command, Task& task, const Options& options) const noexcept {
        static string commandLineCommandKey("command-line-command");
        static string commandLineKey("command-line");

        const SettingsNode& rootSettings = options.getSettings(commandLineCommandKey);
        const SettingsNode settings = getContainingSettings(command, rootSettings, commandLineKey); 
        const TaskCollection tasks = settings[commandLineKey].toStringCollection();

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            TaskCollection combinationTask = tasks;
            replacePatternCombinations(combinationTask, combination);

            for(const auto& commandLine : combinationTask) {
                Task forkedTask = task;
                forkedTask.append(commandLine);
                registerTask(forkedTask, options);
            }
        }
        return true;
    }
} }
