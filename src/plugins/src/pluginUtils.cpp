#include "pluginUtils.h"

#include <map>
#include <utility>
#include <vector>

#include "config/environment.h"
#include "config/path.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/patterns.h"

#include "bootstrap.h"
#include "clangStaticAnalyzer.h"
#include "commandLineCommand.h"
#include "cppcheck.h"
#include "logger.h"
#include "make.h"
#include "scons.h"

using std::map;
using std::ostream;
using std::string;
using std::vector;

using execHelper::config::Command;
using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Path;
using execHelper::config::PatternCombinations;
using execHelper::config::PatternKey;
using execHelper::config::PatternKeys;
using execHelper::config::Patterns;
using execHelper::config::PatternValue;
using execHelper::config::PatternValues;
using execHelper::config::SettingsKeys;
using execHelper::config::VariablesMap;
using execHelper::core::replacePatterns;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::plugins::PatternPermutator;

namespace execHelper::plugins {
auto getPatternsKey() noexcept -> const PatternKey& {
    static const PatternKey key("patterns");
    return key;
}

auto makePatternPermutator(const Patterns& patterns) noexcept
    -> PatternPermutator {
    std::map<PatternKey, PatternValues> patternValuesMap;
    if(patterns.empty()) {
        // If no patterns were given, iterate once
        patternValuesMap.emplace("NO-KEY", PatternValues({"NO-VALUE"}));
    }
    for(const auto& pattern : patterns) {
        patternValuesMap.emplace(pattern.getKey(), pattern.getValues());
    }
    return plugins::PatternPermutator(patternValuesMap);
}

auto replacePatternsInEnvironment(
    const EnvironmentCollection& env,
    const PatternCombinations& patternCombinations) noexcept
    -> EnvironmentCollection {
    EnvironmentCollection replaced;

    for(const auto& keyValue : env) {
        auto key = keyValue.first;
        auto value = keyValue.second;

        for(const auto& pattern : patternCombinations) {
            key = replacePatterns(key, pattern.first, pattern.second);
            value = replacePatterns(value, pattern.first, pattern.second);
        }
        replaced.emplace(std::make_pair(key, value));
    }
    return replaced;
}

auto replacePatternCombinations(
    const Task& task, const PatternCombinations& patternCombinations) noexcept
    -> Task {
    Task replacedTask;
    replacedTask.setEnvironment(replacePatternsInEnvironment(
        task.getEnvironment(), patternCombinations));

    string newWorkingDir = task.getWorkingDirectory().string();
    for(const auto& pattern : patternCombinations) {
        newWorkingDir =
            replacePatterns(newWorkingDir, pattern.first, pattern.second);
    }
    replacedTask.setWorkingDirectory(newWorkingDir);

    for(auto argument : task.getTask()) {
        for(const auto& pattern : patternCombinations) {
            argument = replacePatterns(argument, pattern.first, pattern.second);
        }
        replacedTask.append(argument);
    }
    return replacedTask;
}

auto getEnvironment(const VariablesMap& variables) noexcept
    -> EnvironmentCollection {
    EnvironmentCollection result;
    SettingsKeys key({ENVIRONMENT_KEY});
    auto environmentOpt = variables.get<vector<string>>(key);
    for(auto variableName : environmentOpt.value_or(vector<string>())) {
        auto variableValueOpt =
            variables.get<string>({ENVIRONMENT_KEY, variableName});
        if(!variableValueOpt) {
            LOG(warning) << "Environment variable '" << variableName
                         << "' does not have an associated value. Ignoring it.";
            continue;
        }
        result.emplace(variableName, move(variableValueOpt.value()));
    }
    return result;
}

auto getWorkingDirKey() noexcept -> const string& {
    static const string workingDirKey("working-dir");
    return workingDirKey;
}

auto toString(const PatternKeys& values) noexcept -> string {
    string result;
    if(values.empty()) {
        return result;
    }
    result.append(values.front());
    for(auto it = values.begin() + 1; it != values.end(); ++it) {
        result.append(", ").append(*it);
    }
    return result;
}
} // namespace execHelper::plugins
