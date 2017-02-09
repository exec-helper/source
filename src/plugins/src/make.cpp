#include "make.h"

#include <string>

#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"
#include "core/patternsHandler.h"

#include "pluginUtils.h"

using std::string;
using execHelper::core::Task;
using execHelper::core::Command;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;
using execHelper::config::SettingsNode;

namespace {
    const string MAKE_COMMAND("make");
}

namespace execHelper { namespace plugins {
    bool Make::apply(const Command& command, Task& task, const Options& options) const noexcept {
        const SettingsNode& rootSettings = options.getSettings({"make"});  
        task.append(MAKE_COMMAND);

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task newTask = task;
            if(getMultiThreaded(command, rootSettings, options)) {
                newTask.append(TaskCollection({"--jobs", "8"}));
            }
            TaskCollection buildTarget = getBuildDir(command, rootSettings, combination);
            if(!buildTarget.empty()) {
                newTask.append("--directory=" + buildTarget[0]);
            }
            if(options.getVerbosity()) {
                newTask.append("--debug");
            }
            newTask.append(getCommandLine(command, rootSettings, combination));

            registerTask(newTask, options);
        }
        return true;
    }
} }
