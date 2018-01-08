#include "pluginUtils.h"

#include <map>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>

#include "config/environment.h"
#include "config/path.h"
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
using std::pair;
using std::string;
using std::vector;

using execHelper::config::Command;
using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Path;
using execHelper::config::PatternCombinations;
using execHelper::config::PatternKey;
using execHelper::config::PatternKeys;
using execHelper::config::PatternValue;
using execHelper::config::PatternValues;
using execHelper::config::Patterns;
using execHelper::config::SettingsKeys;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::replacePatterns;
using execHelper::plugins::PatternPermutator;

namespace execHelper {
namespace plugins {
const PatternKey& getPatternsKey() noexcept {
    static const PatternKey key("patterns");
    return key;
}

PatternPermutator makePatternPermutator(const Patterns& patterns) noexcept {
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

Task replacePatternCombinations(
    const Task& task, const PatternCombinations& patternCombinations) noexcept {
    Task replacedTask;
    for(pair<std::string, std::string> environment : task.getEnvironment()) {
        for(const auto& pattern : patternCombinations) {
            environment.first = replacePatterns(environment.first,
                                                pattern.first, pattern.second);
            environment.second = replacePatterns(environment.second,
                                                 pattern.first, pattern.second);
        }
        replacedTask.appendToEnvironment(std::move(environment));
    }
    string newWorkingDir = task.getWorkingDirectory().native();
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

EnvironmentCollection getEnvironment(const VariablesMap& variables) noexcept {
    EnvironmentCollection result;
    SettingsKeys key({ENVIRONMENT_KEY});
    auto environmentOpt = variables.get<vector<string>>(key);
    if(!environmentOpt) {
        return result;
    }

    for(auto variableName : environmentOpt.get()) {
        auto variableValueOpt =
            variables.get<string>({ENVIRONMENT_KEY, variableName});
        if(!variableValueOpt) {
            LOG(warning) << "Environment variable '" << variableName
                         << "' does not have an associated value. Ignoring it.";
            continue;
        }
        result.emplace(variableName, move(variableValueOpt.get()));
    }
    return result;
}

const string& getWorkingDirKey() noexcept {
    static const string workingDirKey("working-dir");
    return workingDirKey;
}

string toString(const PatternKeys& values) noexcept {
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
} // namespace plugins
} // namespace execHelper
