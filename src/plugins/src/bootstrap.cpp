#include "bootstrap.h"

#include <string>
#include <iostream>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"
#include "core/patternsHandler.h"

#include "pluginUtils.h"

using std::string;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;
using execHelper::core::Command;
using execHelper::config::SettingsNode;

namespace execHelper { namespace plugins {
    bool Bootstrap::apply(const Command& command, Task& task, const Options& options) const noexcept {
        static string bootstrapKey("bootstrap");
        const SettingsNode& rootSettings = options.getSettings(bootstrapKey);  
        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task bootstrapTask = task;
            TaskCollection buildDir = getBuildDir(command, rootSettings, combination);
            for(const auto& argument : buildDir) {
                bootstrapTask.append("cd");
                bootstrapTask.append(argument);
                bootstrapTask.append("&&");
            }

            string filename = getBootstrapFilename(command, rootSettings);
            bootstrapTask.append(string("./") + filename);

            registerTask(bootstrapTask, options);
        }
        return true;
    }

    string Bootstrap::getBootstrapFilename(const Command& command, const SettingsNode& rootSettings) noexcept {
        static string filenameKey("filename");
        const SettingsNode& settings = getContainingSettings(command, rootSettings, filenameKey);
        if(settings.contains(filenameKey)) {
            TaskCollection commandArguments = settings[filenameKey].toStringCollection();
            if(commandArguments.size() > 0U) {
                return commandArguments[0];
            }
        }
        return "bootstrap.sh";
    }
} }
