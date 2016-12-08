#include "pluginUtils.h"

#include "config/settingsNode.h"
#include "core/patterns.h"

using std::string;
using execHelper::config::SettingsNode;
using execHelper::core::Command;
using execHelper::core::TaskCollection;
using execHelper::core::CompilerDescriptionElement;
using execHelper::core::Patterns;

namespace execHelper { namespace plugins {
    const string& getPatternsKey() noexcept {
        static const string patternsKey("patterns");
        return patternsKey;
    }

    const SettingsNode& getContainingSettings(const Command& command, const SettingsNode& rootSettings, const string& key) noexcept {
        if(rootSettings.contains(command) && rootSettings[command].contains(key)) {
            return rootSettings[command]; 
        }
        return rootSettings;
    }

    TaskCollection getCommandLine(const Command& command, const SettingsNode& rootSettings, const CompilerDescriptionElement& compiler) noexcept {
        static const string commandLineKey("command-line");
        const SettingsNode settings = getContainingSettings(command, rootSettings, commandLineKey); 
        if(! settings.contains(commandLineKey)) {
            return TaskCollection();
        }

        TaskCollection commandArguments = settings[commandLineKey].toStringCollection();
        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
        Patterns patterns = patternSettings[getPatternsKey()].toStringCollection();
        for(auto& argument : commandArguments) {
            argument = replacePatterns(argument, patterns, compiler);
        }
        return commandArguments;
    }




} }
