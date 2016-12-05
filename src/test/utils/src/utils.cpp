#include "utils.h"

#include "config/settingsNode.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;

namespace {
    const string YAML_CONFIG_KEY_DELIMITER(": ");
    const string YAML_CONFIG_DELIMITER("\n");
    const string YAML_CONFIG_OPTION_CHARACTER("    - ");
}

namespace execHelper { namespace test { namespace utils {
   MainVariables::MainVariables(const vector<string>& arguments) {
        argc = static_cast<int>(arguments.size());
        argv.reset(new char*[argc]);

        for(size_t i = 0; i < arguments.size(); ++i) {
            argv.get()[i] = const_cast<char*>(arguments[i].c_str());
        }
    }

    string convertToConfig(const string& key, const vector<string>& values) {
        string config;
        config += key + YAML_CONFIG_KEY_DELIMITER;
        for(const auto& value : values) {
            config += YAML_CONFIG_DELIMITER + YAML_CONFIG_OPTION_CHARACTER + value;
        }
        config += YAML_CONFIG_DELIMITER;
        config += YAML_CONFIG_DELIMITER;
        return config;
    }

    string convertToConfig(string key, string value) {
        return string(key + YAML_CONFIG_KEY_DELIMITER + value) + YAML_CONFIG_DELIMITER + YAML_CONFIG_DELIMITER;
    }

    string convertToConfig(const string& key, const std::initializer_list<string>& values) {
        std::vector<string> vectorValues(values);
        return convertToConfig(key, vectorValues);
    }

    string basename(const string& file) {
        unsigned int found = file.find_last_of("/\\");
        return file.substr(0,found);
    }

    void addSettings(SettingsNode& settings, const string& key, const string& value) noexcept {
        addSettings(settings, key, {value});
    }

    void addSettings(SettingsNode& settings, const string& key, const std::initializer_list<string>& values) noexcept {
        addSettings(settings, key, vector<string>(values));
    }

    void addSettings(SettingsNode& settings, const string& key, const std::vector<string>& values) noexcept {
        SettingsNode& settingsToWriteTo = getSetting(settings, key);
        for(const auto& value : values) {
            SettingsNode valueSetting;
            valueSetting.m_key = value;
            settingsToWriteTo.m_values.emplace_back(valueSetting);
        }
    }

    SettingsNode& getSetting(SettingsNode& settings, const string& key) noexcept {
        if(settings.contains(key)) {
            return settings[key];
        }

        settings.m_values.emplace_back(SettingsNode());
        settings.m_values.back().m_key = key;
        return settings.m_values.back();
    }
} } }
