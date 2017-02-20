#include "make.h"

#include <string>

#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"
#include "core/patternsHandler.h"

#include "pluginUtils.h"
#include "configValue.h"

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

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task makeTask = replacePatternCombinations(task, combination);
            registerTask(makeTask, options);
        }
        return true;
    }
} }
