#include "buildPlugin.h"

#include <string>

#include "config/settingsNode.h"
#include "core/compilerDescription.h"
#include "core/patterns.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::TaskCollection;
using execHelper::core::Command;
using execHelper::core::CompilerDescriptionElement;
using execHelper::core::Patterns;
using execHelper::core::replacePatterns;

namespace {
    const string patternsKey("patterns");
}

namespace execHelper { namespace plugins {
    TaskCollection BuildPlugin::getCommandLine(const Command& command, const SettingsNode& rootSettings, const CompilerDescriptionElement& compiler) noexcept {
        static const string commandLineKey("command-line");
        const SettingsNode settings = getContainingSettings(command, rootSettings, commandLineKey); 
        if(! settings.contains(commandLineKey)) {
            return TaskCollection();
        }

        TaskCollection commandArguments = settings[commandLineKey].toStringCollection();
        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, patternsKey); 
        Patterns patterns = patternSettings[patternsKey].toStringCollection();
        for(auto& argument : commandArguments) {
            argument = replacePatterns(argument, patterns, compiler);
        }
        return commandArguments;
    }

    const SettingsNode& BuildPlugin::getContainingSettings(const Command& command, const SettingsNode& rootSettings, const string& key) noexcept {
        if(rootSettings.contains(command) && rootSettings[command].contains(key)) {
            return rootSettings[command]; 
        }
        return rootSettings;
    }

    TaskCollection BuildPlugin::getMultiThreaded(const std::string& command, const SettingsNode& rootSettings) noexcept {
        static const string singleThreadedKey("single-threaded");
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
        
        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, patternsKey); 
        Patterns patterns = patternSettings[patternsKey].toStringCollection();
        for(const auto& argument : commandArguments) {
            result.push_back(replacePatterns(argument, patterns, compiler));
        }
        return result;
    }
} }

