#include "make.h"

#include <string>

#include "config/settingsNode.h"
#include "core/patterns.h"
#include "core/patternsHandler.h"
#include "core/task.h"

#include "configValue.h"
#include "pluginUtils.h"

using std::string;
using execHelper::core::Task;
using execHelper::core::Command;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;
using execHelper::config::SettingsNode;

namespace execHelper { namespace plugins {
    bool Make::apply(const Command& command, Task task, const Options& options) const noexcept {
        static const string MAKE_COMMAND("make");

        const SettingsNode& rootSettings = options.getSettings({"make"});  
        task.append(MAKE_COMMAND);

        if(getMultiThreaded(command, rootSettings, options)) {
            task.append(TaskCollection({"--jobs", "8"}));
        }
        string buildDir = ConfigValue<string>::get(getBuildDirKey(), "", command, rootSettings);
        if(!buildDir.empty()) {
            task.append("--directory=" + buildDir);
        }
        if(options.getVerbosity()) {
            task.append("--debug");
        }
        task.append(ConfigValue<TaskCollection>::get(getCommandLineKey(), {}, command, rootSettings));
        task.appendToEnvironment(getEnvironment(command, rootSettings));

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task makeTask = replacePatternCombinations(task, combination);
            if(! registerTask(makeTask, options)) {
                return false;
            }
        }
        return true;
    }
} }
