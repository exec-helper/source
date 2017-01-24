#include "pluginUtils.h"

#include <vector>
#include <map>

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
using std::vector;
using std::map;

using execHelper::config::SettingsNode;
using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::PatternCombinations;
using execHelper::core::PatternsHandler;
using execHelper::core::replacePatterns;
using execHelper::core::PatternPermutator;
using execHelper::core::PatternKeys;
using execHelper::core::PatternKey;
using execHelper::core::PatternValue;
using execHelper::core::Options;

namespace execHelper { namespace plugins {
    const string& getPatternsKey() noexcept {
        static const string patternsKey("patterns");
        return patternsKey;
    }

    Task replacePatternCombinations(const Task& task, const PatternCombinations& patternCombinations) noexcept {
        Task replacedTask;
        for(const auto& part : task.getTask()) {
            auto argument = part;
            for(const auto& pattern : patternCombinations) {
                argument = replacePatterns(part, pattern.first, pattern.second);
            }
            replacedTask.append(part);
        }
        return task;
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

    TaskCollection getCommandLine(const Command& command, const SettingsNode& rootSettings) noexcept {
        static const string commandLineKey("command-line");
        const SettingsNode settings = getContainingSettings(command, rootSettings, commandLineKey); 
        if(! settings.contains(commandLineKey)) {
            return TaskCollection();
        }
        TaskCollection commandArguments = settings[commandLineKey].toStringCollection();
        return commandArguments;
    }

    TaskCollection getCommandLine(const Command& command, const SettingsNode& rootSettings, const PatternCombinations& patternCombinations) noexcept {
        TaskCollection commandArguments = getCommandLine(command, rootSettings);
        replacePatternCombinations(commandArguments, patternCombinations);
        return commandArguments;
    }

    boost::optional<string> getConfigurationSetting(const string& command, const SettingsNode& rootSettings, const string& configKey, const string& prepend) noexcept {
        boost::optional<TaskCollection> collection = getConfigurationSettings(command, rootSettings, configKey);
        if(collection == boost::none) {
            return boost::none;
        }
        if(collection.get().empty()) {
            return std::string();
        }
        return prepend + collection.get().back();
    }

    boost::optional<TaskCollection> getConfigurationSettings(const string& command, const SettingsNode& rootSettings, const string& configKey) noexcept {
        const SettingsNode& settings = getContainingSettings(command, rootSettings, configKey);
        if(!settings.contains(configKey)) {
            return boost::none;
        }
        return settings[configKey].toStringCollection();
    }

    PatternPermutator makePatternPermutator(const Command& command, const SettingsNode& rootSettings, const Options& options) noexcept {
        boost::optional<PatternKeys> patternKeys = getConfigurationSettings(command, rootSettings, getPatternsKey());
        if(patternKeys == boost::none) {
            // If no patterns are defined, make sure to iterate once
            static map<PatternKey, vector<PatternValue>> noKeysMap({{"NOKEY", {"NOVALUE"}}});
            return PatternPermutator(noKeysMap);
        }
        for(const auto& pattern : patternKeys.get()) {
            if(!options.getPatternsHandler().contains(pattern)) {
                user_feedback_error("Error: pattern with key '" << pattern << "' not configured");
                return PatternPermutator({});
            }
        }
        return options.makePatternPermutator(patternKeys.get());
    }
} }
