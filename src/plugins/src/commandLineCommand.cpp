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

        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
        PatternKeys patterns; 
        if(patternSettings.contains(getPatternsKey())) {
            patterns = patternSettings[getPatternsKey()].toStringCollection();
        }
        for(const auto& combination : options.makePatternPermutator(patterns)) {
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
