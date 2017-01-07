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
        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
        PatternKeys patterns; 
        if(patternSettings.contains(getPatternsKey())) {
            patterns = patternSettings[getPatternsKey()].toStringCollection();
        }
        for(const auto& combination : options.makePatternPermutator(patterns)) {
            //for(const auto& target : options.getTarget()) {
                Task newTask = task;
                if(getMultiThreaded(command, rootSettings, options)) {
                    newTask.append(TaskCollection({"--jobs", "8"}));
                }
                TaskCollection buildTarget = getBuildDir(command, rootSettings, combination);
                if(!buildTarget.empty()) {
                    newTask.append("--directory=" + buildTarget[0]);
                }
                newTask.append(getCommandLine(command, rootSettings, combination));

                //string targetName = target.getTarget();
                //string runTargetName = target.getRunTarget();
                //newTask.append(getTarget(command, rootSettings, target));
                registerTask(newTask, options);
            //}
        }
        return true;
    }
} }
