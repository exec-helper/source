#include "utils.h"

#include <filesystem>
#include <iostream>
#include <sstream>

#include <gsl/gsl>

#include "config/pattern.h"
#include "config/settingsNode.h"
#include "plugins/pluginUtils.h"

#include "base-utils/configFileWriter.h"
#include "base-utils/yaml.h"

#include "logger.h"

using std::endl;
using std::initializer_list;
using std::map;
using std::pair;
using std::reference_wrapper;
using std::string;
using std::stringstream;
using std::vector;

using gsl::not_null;

using execHelper::config::EnvironmentCollection;
using execHelper::config::EnvironmentValue;
using execHelper::config::Path;
using execHelper::config::Pattern;
using execHelper::config::PatternCombinations;
using execHelper::config::PatternKey;
using execHelper::config::PatternKeys;
using execHelper::config::PatternValue;
using execHelper::config::PatternValues;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::SettingsValues;
using execHelper::core::Task;
using execHelper::plugins::PatternPermutator;
using execHelper::plugins::replacePatternCombinations;

using execHelper::core::test::ExecutorStub;
using execHelper::test::baseUtils::ConfigFileWriter;
using execHelper::test::baseUtils::YamlWriter;

namespace filesystem = std::filesystem;

namespace {
const string YAML_CONFIG_KEY_DELIMITER(": ");
const string YAML_CONFIG_DELIMITER("\n");
const string YAML_CONFIG_NESTED_CHARACTER("    ");
const string YAML_CONFIG_OPTION_CHARACTER("- ");

const string rootPatternKey("patterns");
} // namespace

namespace execHelper {
namespace test {
namespace utils {
MainVariables::MainVariables(const Arguments& arguments) {
    argc = static_cast<int>(arguments.size());
    argv.reset(new char*[argc]);

    for(size_t i = 0; i < arguments.size(); ++i) {
        argv.get()[i] = const_cast<char*>(arguments[i].c_str());
    }
}

string convertToConfig(const initializer_list<string>& keys,
                       const string& value, const string& prepend) noexcept {
    return convertToConfig(vector<string>(keys), {value}, prepend);
}

string convertToConfig(const initializer_list<string>& keys,
                       const initializer_list<string>& values,
                       const string& prepend) noexcept {
    return convertToConfig(vector<string>(keys), values, prepend);
}

string convertToConfig(const initializer_list<string>& keys,
                       const vector<string>& values,
                       const string& prepend) noexcept {
    return convertToConfig(vector<string>(keys), vector<string>(values),
                           prepend);
}

string convertToConfig(const vector<string>& keys,
                       const initializer_list<string>& values,
                       const string& prepend) noexcept {
    return convertToConfig(keys, vector<string>(values), prepend);
}

string convertToConfig(const vector<string>& keys, const vector<string>& values,
                       const string& prepend) noexcept {
    string config;

    string nestedCharacters;
    for(const auto& key : keys) {
        config += prepend + nestedCharacters + key + YAML_CONFIG_KEY_DELIMITER +
                  YAML_CONFIG_DELIMITER;
        nestedCharacters += YAML_CONFIG_NESTED_CHARACTER;
    }
    for(const auto& value : values) {
        config += prepend + nestedCharacters + YAML_CONFIG_OPTION_CHARACTER +
                  value + YAML_CONFIG_DELIMITER;
    }
    config += YAML_CONFIG_DELIMITER;
    return config;
}

string convertToConfig(const string& key, const vector<string>& values,
                       const string& prepend) {
    return convertToConfig(vector<string>({key}), values, prepend);
}

string convertToConfig(const Patterns& patterns) noexcept {
    stringstream stream;

    if(!patterns.empty()) {
        stream << rootPatternKey << YAML_CONFIG_KEY_DELIMITER
               << YAML_CONFIG_DELIMITER;
        for(auto& pattern : patterns) {
            string patternNestedCharacter = YAML_CONFIG_NESTED_CHARACTER;
            stream << patternNestedCharacter << pattern.getKey()
                   << YAML_CONFIG_KEY_DELIMITER << YAML_CONFIG_DELIMITER;

            patternNestedCharacter += YAML_CONFIG_NESTED_CHARACTER;
            const auto shortOption = pattern.getShortOption();
            if(shortOption) {
                stream << convertToConfig("short-option",
                                          string(1U, shortOption.value()),
                                          patternNestedCharacter);
            }
            const auto longOption = pattern.getLongOption();
            if(longOption) {
                stream << convertToConfig("long-option", longOption.value(),
                                          patternNestedCharacter);
            }
            stream << convertToConfig("values", pattern.getValues(),
                                      patternNestedCharacter);
        }
    }
    return stream.str();
}

string convertToConfig(const SettingsNode& settings,
                       const string& prepend) noexcept {
    stringstream stream;
    const string nestedCharacter(prepend);

    const auto settingsValues =
        settings.get<SettingsValues>(SettingsKeys(), SettingsValues());
    if(settingsValues.empty()) {
        stream << prepend << YAML_CONFIG_OPTION_CHARACTER << settings.key()
               << YAML_CONFIG_DELIMITER;
    } else if(settingsValues.size() == 1 &&
              settings
                  .get<SettingsValues>(settingsValues.back(), SettingsValues())
                  .empty()) {
        stream << prepend << settings.key() << YAML_CONFIG_KEY_DELIMITER
               << settingsValues.back() << YAML_CONFIG_DELIMITER;
    } else {
        string valueNestedCharacter = nestedCharacter;
        stream << nestedCharacter << settings.key() << YAML_CONFIG_KEY_DELIMITER
               << YAML_CONFIG_DELIMITER;
        valueNestedCharacter += YAML_CONFIG_NESTED_CHARACTER;
        for(const auto& key : settingsValues) {
            stream << convertToConfig(settings[key], valueNestedCharacter);
        }
    }
    return stream.str();
}

string convertToConfig(const SettingsNode& settings, const Patterns& patterns,
                       const string& prepend) noexcept {
    string result = convertToConfig(patterns);
    result += convertToConfig(settings, prepend);
    return result;
}

string convertToConfig(string key, string value, const string& prepend) {
    return prepend + key + YAML_CONFIG_KEY_DELIMITER + value +
           YAML_CONFIG_DELIMITER;
}

string convertToConfig(const string& key,
                       const std::initializer_list<string>& values,
                       const string& prepend) {
    std::vector<string> vectorValues(values);
    return convertToConfig(key, vectorValues, prepend);
}

string basename(const string& file) {
    auto found = file.find_last_of("/\\");
    return file.substr(0, found);
}

YamlWriter toYaml(const SettingsNode& settings,
                  const Patterns& patterns) noexcept {
    static string patternKey("patterns");
    YamlWriter yaml;
    try {
        for(const auto& pattern : patterns) {
            const auto longOption = pattern.getLongOption();
            if(longOption) {
                yaml[patternKey][pattern.getKey()]["long-option"] =
                    longOption.value();
            }
            yaml[patternKey][pattern.getKey()]["default-values"] =
                pattern.getValues();
            const auto shortOption = pattern.getShortOption();
            if(shortOption) {
                yaml[patternKey][pattern.getKey()]["short-option"] =
                    shortOption.value();
            }
        }
        const auto settingsValues =
            settings.get<SettingsValues>(SettingsKeys(), SettingsValues());
        for(const auto& subKey : settingsValues) {
            if(settings.get<SettingsValues>(subKey, SettingsValues()).empty()) {
                if(settingsValues.size() == 1U) {
                    yaml = subKey;
                } else {
                    yaml.push_back(subKey);
                }
            } else {
                yaml[subKey] = toYaml(settings[subKey], Patterns());
            }
        }
    } catch(const YAML::InvalidNode&) {
        LOG(error) << "Somehow the used YAML node is invalid";
        assert(false);
    }
    return yaml;
}

void writeSettingsFile(not_null<ConfigFileWriter*> configFileWriter,
                       const SettingsNode& settings,
                       const Patterns& patterns) noexcept {
    YamlWriter yaml = toYaml(settings, patterns);
    configFileWriter->write(yaml);
}

PatternCombinations createPatternCombination(
    const initializer_list<PatternKey>& keys,
    const initializer_list<PatternValue>& values) noexcept {
    return createPatternCombination(PatternKeys(keys), values);
}

PatternCombinations
createPatternCombination(const PatternKeys& keys,
                         const PatternValues& values) noexcept {
    PatternCombinations combination;
    if(keys.size() != values.size()) {
        return combination;
    }

    for(size_t i = 0; i < keys.size(); ++i) {
        combination[keys[i]] = values[i];
    }
    return combination;
}

PatternPermutator makePatternPermutator(const Patterns& patterns) noexcept {
    map<PatternKey, PatternValues> patternValuesMatrix;
    if(patterns.empty()) {
        patternValuesMatrix.emplace(
            make_pair(string("BLAATBLAATBLAATBLAAT"), vector<string>({"b"})));
    } else {
        for(const auto& pattern : patterns) {
            patternValuesMatrix.emplace(
                make_pair(pattern.getKey(), pattern.getValues()));
        }
    }
    return PatternPermutator(patternValuesMatrix);
}

ExecutorStub::TaskQueue getExpectedTasks(const Task& task,
                                         const Patterns patterns) noexcept {
    ExecutorStub::TaskQueue result;
    for(const auto& combination : makePatternPermutator(patterns)) {
        result.emplace_back(replacePatternCombinations(task, combination));
    }
    return result;
}

ExecutorStub::TaskQueue getExpectedTasks(const ExecutorStub::TaskQueue& tasks,
                                         const Patterns patterns) noexcept {
    ExecutorStub::TaskQueue result;
    for(const auto& combination : makePatternPermutator(patterns)) {
        for(const auto& task : tasks) {
            result.emplace_back(replacePatternCombinations(task, combination));
        }
    }
    return result;
}

string toString(const SettingsNode& settings, unsigned int nbOfTabs) noexcept {
    string prefix;
    for(unsigned int i = 0; i < nbOfTabs; ++i) {
        prefix += "  ";
    }

    string result;
    result += prefix + "- " + settings.key();
    auto settingsValues = settings.get<SettingsValues>(SettingsKeys());
    if(settingsValues) {
        result += "\n";
        return result;
    } else {
        result += ":\n";
    }
    for(const auto& value : settingsValues.value()) {
        result += toString(settings[value], nbOfTabs + 1);
    }
    return result;
}

std::string inheritWorkingDirKey() noexcept { return "EH_WORKING_DIR"; }

Patterns getPredefinedPatterns() noexcept {
    Patterns predefined;
    predefined.emplace_back(
        Pattern{inheritWorkingDirKey(), {filesystem::current_path().string()}});
    return predefined;
}
} // namespace utils
} // namespace test
} // namespace execHelper
