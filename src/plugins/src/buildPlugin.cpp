#include "buildPlugin.h"

#include <string>

#include "config/settingsNode.h"
#include "core/compilerDescription.h"
#include "core/patterns.h"
#include "core/options.h"

#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Command;
using execHelper::core::CompilerDescriptionElement;
using execHelper::core::Patterns;
using execHelper::core::replacePatterns;

namespace execHelper { namespace plugins {
    TaskCollection BuildPlugin::getCommandLine(const Command& command, const SettingsNode& rootSettings, const CompilerDescriptionElement& compiler) noexcept {
        return ::execHelper::plugins::getCommandLine(command, rootSettings, compiler);
    }

    TaskCollection BuildPlugin::getMultiThreaded(const std::string& command, const SettingsNode& rootSettings, const Options& options) noexcept {
        static const string singleThreadedKey("single-threaded");

        if(options.getSingleThreaded()) {
            return TaskCollection();
        }

        const SettingsNode settings = getContainingSettings(command, rootSettings, singleThreadedKey); 
        if(settings.contains(singleThreadedKey)) {
            TaskCollection commandArguments = settings[singleThreadedKey].toStringCollection();
            if(commandArguments.size() > 0 && commandArguments[0] == "yes") {
                return TaskCollection();
            }
        }
        return TaskCollection({"--jobs", "8"});
    }

    TaskCollection BuildPlugin::getBuildDir(const Command& command, const SettingsNode& rootSettings, const CompilerDescriptionElement& compiler) noexcept {
        static const string buildDirKey("build-dir");

        TaskCollection result;
        const SettingsNode settings = getContainingSettings(command, rootSettings, buildDirKey); 
        if(! settings.contains(buildDirKey)) {
            return result;
        }
        TaskCollection commandArguments = settings[buildDirKey].toStringCollection();
        
        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
        Patterns patterns = patternSettings[getPatternsKey()].toStringCollection();
        for(const auto& argument : commandArguments) {
            result.push_back(replacePatterns(argument, patterns, compiler));
        }
        return result;
    }
} }

