#include "buildPlugin.h"

#include <string>

#include "config/settingsNode.h"
#include "core/patterns.h"
#include "core/options.h"

#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Command;
using execHelper::core::replacePatterns;
using execHelper::core::PatternCombinations;

namespace execHelper { namespace plugins {
    const std::string& BuildPlugin::getBuildDirKey() noexcept {
        static const string buildDirKey("build-dir");
        return buildDirKey;
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

    TaskCollection BuildPlugin::getBuildDir(const Command& command, const SettingsNode& rootSettings, const PatternCombinations patternCombinations) noexcept {
        const string buildDirKey = getBuildDirKey();

        TaskCollection result;
        const SettingsNode settings = getContainingSettings(command, rootSettings, buildDirKey); 
        if(! settings.contains(buildDirKey)) {
            return result;
        }
        TaskCollection commandArguments = settings[buildDirKey].toStringCollection();
        replacePatternCombinations(commandArguments, patternCombinations);
        return commandArguments;
    }

    //TaskCollection BuildPlugin::getTarget(const Command& command, const SettingsNode& rootSettings, const TargetDescriptionElement& target) noexcept {
        //static const string targetKey("target");

        //TaskCollection result;
        //const SettingsNode settings = getContainingSettings(command, rootSettings, targetKey); 
        //if(! settings.contains(targetKey)) {
            //return TaskCollection({target.getTarget() + target.getRunTarget()});
        //}
        //TaskCollection commandArguments = settings[targetKey].toStringCollection();

        //const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
        //Patterns patterns = patternSettings[getPatternsKey()].toStringCollection();
        //for(const auto& argument : commandArguments) {
            //string replacedString(argument);
            //replacedString = replacePatterns(replacedString, patterns, target);
            //result.push_back(replacedString);
        //}
        //return result;
    //}
} }

