#include "bootstrap.h"

#include <string>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/compilerDescription.h"
#include "core/patterns.h"

#include "pluginUtils.h"

using std::string;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::CompilerDescriptionElement;
using execHelper::core::Patterns;
using execHelper::core::Command;
using execHelper::config::SettingsNode;

namespace execHelper { namespace plugins {
    bool Bootstrap::apply(const Command& command, Task& task, const Options& options) const noexcept {
        static string bootstrapKey("bootstrap");
        const SettingsNode& rootSettings = options.getSettings(bootstrapKey);  
        for(const auto& compiler : options.getCompiler()) {
            Task bootstrapTask = task;
            TaskCollection buildDir = getBuildDir(command, rootSettings, compiler);
            for(const auto& argument : buildDir) {
                const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
                Patterns patterns = patternSettings[getPatternsKey()].toStringCollection();
                string replacedBuildDir = replacePatterns(argument, patterns, compiler);

                bootstrapTask.append("cd");
                bootstrapTask.append(replacedBuildDir);
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
