#include "utils.h"

using std::vector;
using std::string;

namespace {
    const std::string YAML_CONFIG_KEY_DELIMITER(": ");
    const std::string YAML_CONFIG_DELIMITER("\n");
    const std::string YAML_CONFIG_OPTION_CHARACTER("    - ");
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
        std::string config;
        config += key + YAML_CONFIG_KEY_DELIMITER;
        for(const auto& value : values) {
            config += YAML_CONFIG_DELIMITER + YAML_CONFIG_OPTION_CHARACTER + value;
        }
        config += YAML_CONFIG_DELIMITER;
        config += YAML_CONFIG_DELIMITER;
        return config;
    }

    string convertToConfig(string key, string value) {
        return std::string(key + YAML_CONFIG_KEY_DELIMITER + value) + YAML_CONFIG_DELIMITER + YAML_CONFIG_DELIMITER;
    }

    string convertToConfig(const std::string& key, const std::initializer_list<std::string>& values) {
        std::vector<std::string> vectorValues(values);
        return convertToConfig(key, vectorValues);
    }

    string basename(const string& file) {
        unsigned int found = file.find_last_of("/\\");
        return file.substr(0,found);
    }

} } }
