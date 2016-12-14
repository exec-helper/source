#include "pluginUtils.h"

#include "config/settingsNode.h"
#include "core/patterns.h"
#include "core/targetDescription.h"

using std::string;
using execHelper::config::SettingsNode;
using execHelper::core::Command;
using execHelper::core::TaskCollection;
using execHelper::core::CompilerDescriptionElement;
using execHelper::core::TargetDescriptionElement;
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

    TaskCollection getCommandLine(const Command& command, const SettingsNode& rootSettings) noexcept {
        static const string commandLineKey("command-line");
        const SettingsNode settings = getContainingSettings(command, rootSettings, commandLineKey); 
        if(! settings.contains(commandLineKey)) {
            return TaskCollection();
        }

        return settings[commandLineKey].toStringCollection();
    }

    TaskCollection getCommandLine(const Command& command, const SettingsNode& rootSettings, const TargetDescriptionElement& target) noexcept {
        TaskCollection commandArguments = getCommandLine(command, rootSettings);
        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
        Patterns patterns = patternSettings[getPatternsKey()].toStringCollection();
        for(auto& argument : commandArguments) {
            argument = replacePatterns(argument, patterns, target);
        }
        return commandArguments;
    }

    TaskCollection getCommandLine(const Command& command, const SettingsNode& rootSettings, const CompilerDescriptionElement& compiler) noexcept {
        TaskCollection commandArguments = getCommandLine(command, rootSettings);
        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
        Patterns patterns = patternSettings[getPatternsKey()].toStringCollection();
        for(auto& argument : commandArguments) {
            argument = replacePatterns(argument, patterns, compiler);
        }
        return commandArguments;
    }
} }
