#include "pluginUtils.h"

#include <map>
#include <utility>
#include <vector>

#include "config/settingsNode.h"
#include "core/patterns.h"
#include "log/log.h"

#include "bootstrap.h"
#include "clangStaticAnalyzer.h"
#include "commandLineCommand.h"
#include "cppcheck.h"
#include "make.h"
#include "scons.h"

#include "configValue.h"

using std::map;
using std::pair;
using std::string;
using std::vector;

using execHelper::config::SettingsNode;
using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::EnvironmentCollection;
using execHelper::core::PatternCombinations;
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

    const string& getCommandLineKey() noexcept {
        static const string commandLineKey("command-line");
        return commandLineKey;
    }

    Task replacePatternCombinations(const Task& task, const PatternCombinations& patternCombinations) noexcept {
        Task replacedTask;
        for(pair<std::string, std::string> environment : task.getEnvironment()) {
            for(const auto& pattern : patternCombinations) {
                environment.first = replacePatterns(environment.first, pattern.first, pattern.second);
                environment.second = replacePatterns(environment.second, pattern.first, pattern.second);
            }
            replacedTask.appendToEnvironment(std::move(environment));
        }
        for(auto argument : task.getTask()) {
            for(const auto& pattern : patternCombinations) {
                argument = replacePatterns(argument, pattern.first, pattern.second);
            }
            replacedTask.append(argument);
        }
        return replacedTask;
    }

    void replacePatternCombinations(TaskCollection& commandArguments, const PatternCombinations& patternCombinations) noexcept {
        for(auto& argument : commandArguments) {
            for(const auto& pattern : patternCombinations) {
                argument = replacePatterns(argument, pattern.first, pattern.second);
            }
        }
    }

    boost::optional<const SettingsNode&> getContainingSettings(const string& key, const SettingsNode& rootSettings, const vector<string>& configKeys) noexcept {
        const SettingsNode* settings = &rootSettings;
        for(const auto& configKey : configKeys) {
            if(!configKey.empty() && !settings->contains(configKey)) {
                return boost::none;
            }
            settings = &((*settings)[configKey]);
        }
        if(settings->contains(key)) {
            return (*settings)[key];
        }
        return boost::none;
    }

    TaskCollection getCommandLine(const Command& command, const SettingsNode& rootSettings) noexcept {
        return ConfigValue<TaskCollection>::get("command-line", {}, command, rootSettings);
    }

    TaskCollection getCommandLine(const Command& command, const SettingsNode& rootSettings, const PatternCombinations& patternCombinations) noexcept {
        TaskCollection commandArguments = getCommandLine(command, rootSettings);
        replacePatternCombinations(commandArguments, patternCombinations);
        return commandArguments;
    }

    const std::string& getEnvironmentKey() noexcept {
        static const string environmentKey("environment");
        return environmentKey;
    }

    core::EnvironmentCollection getEnvironment(const core::Command& command, const config::SettingsNode& rootSettings) noexcept {
        EnvironmentCollection result;
        boost::optional<const SettingsNode&> environmentSettingsOpt = ConfigValue<const SettingsNode&>::getSetting(getEnvironmentKey(), rootSettings, {{command}, {}});
        if(environmentSettingsOpt == boost::none) {
            return result;
        }

        const SettingsNode& environmentSettings = environmentSettingsOpt.get();
        for(const auto& setting : environmentSettings.values()) {
            auto environmentSettingValues = environmentSettings[setting].values();
            if(!environmentSettingValues.empty()) {
                result.emplace(make_pair(setting, environmentSettings[setting].values().back()));
            }
        }
        return result;
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
        return ConfigValue<TaskCollection>::getSetting(configKey, rootSettings, {{command}, {}});
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
