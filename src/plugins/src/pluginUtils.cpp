#include "pluginUtils.h"

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/patterns.h"

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
using execHelper::core::PatternCombinations;
using execHelper::core::PatternsHandler;
using execHelper::core::replacePatterns;

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

    void replacePatternCombinations(TaskCollection& commandArguments, const PatternCombinations& patternCombinations) noexcept {
        for(auto& argument : commandArguments) {
            for(const auto& pattern : patternCombinations) {
                argument = replacePatterns(argument, pattern.first, pattern.second);
            }
        }
    }

    const SettingsNode& getContainingSettings(const Command& command, const SettingsNode& rootSettings, const string& key) noexcept {
        if(rootSettings.contains(command) && rootSettings[command].contains(key)) {
            return rootSettings[command]; 
        }
        return rootSettings;
    }

    SettingsNode::SettingsKey getSetting(const SettingsNode& settings, const SettingsNode::SettingsKey& key, const SettingsNode::SettingsKey& defaultValue) noexcept {
        const SettingsNode::SettingsCollection settingsKey = getSettings(settings, key, {defaultValue});
        if(settingsKey.empty()) {
            return defaultValue;
        }
        return settingsKey.back();
    }

    SettingsNode::SettingsCollection getSettings(const SettingsNode& settings, const SettingsNode::SettingsKey& key, const SettingsNode::SettingsCollection& defaultValues) noexcept {
        if(! settings.contains(key)) {
            return defaultValues;
        }
        return settings[key].toStringCollection();
    }

    TaskCollection getCommandLine(const Command& command, const SettingsNode& rootSettings, const PatternCombinations& patternCombinations) noexcept {
        static const string commandLineKey("command-line");
        const SettingsNode settings = getContainingSettings(command, rootSettings, commandLineKey); 
        if(! settings.contains(commandLineKey)) {
            return TaskCollection();
        }
        TaskCollection commandArguments = settings[commandLineKey].toStringCollection();
        replacePatternCombinations(commandArguments, patternCombinations);
        return commandArguments;
    }
} }
