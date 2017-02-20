#include "bootstrap.h"

#include <string>
#include <iostream>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"
#include "core/patternsHandler.h"

#include "pluginUtils.h"
#include "configValue.h"

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

        string buildDir = ConfigValue<string>::get(getBuildDirKey(), "", command, rootSettings);
        if(! buildDir.empty()) {
            const string changeDirectoryCommand = ConfigValue<string>::get("change-directory-command", "cd", command, rootSettings);
            const string chainCommandsCommand = ConfigValue<string>::get("chain-commands-command", "&&", command, rootSettings);

            task.append(changeDirectoryCommand);
            task.append(buildDir);
            task.append(chainCommandsCommand);
        }

        task.append(getBootstrapFilename(command, rootSettings));
        task.append(ConfigValue<TaskCollection>::get(getCommandLineKey(), {}, command, rootSettings));

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task bootstrapTask = replacePatternCombinations(task, combination);
            registerTask(bootstrapTask, options);
        }
        return true;
    }

    string Bootstrap::getBootstrapFilename(const Command& command, const SettingsNode& rootSettings) noexcept {
        static string filenameKey("filename");
        const string filename = ConfigValue<string>::get(filenameKey, "bootstrap.sh", command, rootSettings);
        // Check naively if filename is an absolute path
        if(filename.compare(0, 1, "/") == 0) {
            return filename;
        }
        return "./" + filename;
    }
} }
