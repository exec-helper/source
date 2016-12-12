#include "buildPlugin.h"

#include <string>

#include "config/settingsNode.h"
#include "core/targetDescription.h"
#include "core/compilerDescription.h"
#include "core/patterns.h"
#include "core/options.h"

#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Command;
using execHelper::core::TargetDescriptionElement;
using execHelper::core::CompilerDescriptionElement;
using execHelper::core::Patterns;
using execHelper::core::replacePatterns;

namespace execHelper { namespace plugins {
    TaskCollection BuildPlugin::getCommandLine(const Command& command, const SettingsNode& rootSettings, const CompilerDescriptionElement& compiler) noexcept {
        return ::execHelper::plugins::getCommandLine(command, rootSettings, compiler);
    }

    bool BuildPlugin::getMultiThreaded(const std::string& command, const SettingsNode& rootSettings, const Options& options) noexcept {
        static const string singleThreadedKey("single-threaded");

        if(options.getSingleThreaded()) {
            return false;
        }

        const SettingsNode settings = getContainingSettings(command, rootSettings, singleThreadedKey); 
        if(settings.contains(singleThreadedKey)) {
            TaskCollection commandArguments = settings[singleThreadedKey].toStringCollection();
            if(commandArguments.size() > 0 && commandArguments[0] == "yes") {
                return false;
            }
        }
        return true;
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

    TaskCollection BuildPlugin::getBuildDir(const Command& command, const SettingsNode& rootSettings, const TargetDescriptionElement& target, const CompilerDescriptionElement& compiler) noexcept {
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
            string replacedString(argument);
            replacedString = replacePatterns(replacedString, patterns, target);
            replacedString = replacePatterns(replacedString, patterns, compiler);
            result.push_back(replacedString);
        }
        return result;
    }
} }

