#include "pluginUtils.h"

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/patterns.h"
#include "core/targetDescription.h"

#include "commandLineCommand.h"
#include "scons.h"
#include "make.h"
#include "bootstrap.h"
#include "cppcheck.h"
#include "clangStaticAnalyzer.h"

using std::string;
using std::shared_ptr;
using std::make_shared;

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

    shared_ptr<Plugin> getPlugin(const string& pluginName) noexcept {
        if(pluginName == "command-line-command") {
            return make_shared<plugins::CommandLineCommand>();
        } else if(pluginName == "scons") {
            return make_shared<plugins::Scons>(); 
        } else if(pluginName == "make") {
            return make_shared<plugins::Make>(); 
        } else if(pluginName == "bootstrap") {
            return make_shared<plugins::Bootstrap>(); 
        } else if(pluginName == "cppcheck") {
            return make_shared<plugins::Cppcheck>();
        } else if(pluginName == "clang-static-analyzer") {
            return make_shared<plugins::ClangStaticAnalyzer>();
        }
        user_feedback("'" << pluginName << "' not registered");
        return shared_ptr<Plugin>();
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
