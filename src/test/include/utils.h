#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include <string>
#include <memory>
#include <vector>

namespace execHelper {
    namespace test {
        template<typename T>
        void appendVectors(T& appendTo, const T& appendFrom) {
            appendTo.insert(std::end(appendTo), std::begin(appendFrom), std::end(appendFrom));
        }

        struct MainVariables {
            int argc;
            std::unique_ptr<char*[]> argv;

            MainVariables(const std::vector<std::string>& arguments) {
                argc = static_cast<int>(arguments.size());
                argv.reset(new char*[argc]);

                for(size_t i = 0; i < arguments.size(); ++i) {
                    argv.get()[i] = const_cast<char*>(arguments[i].c_str());
                }
            }
        };

        const std::string YAML_CONFIG_KEY_DELIMITER(": ");
        const std::string YAML_CONFIG_DELIMITER("\n");
        const std::string YAML_CONFIG_OPTION_CHARACTER("    - ");

        std::string convertToConfig(const std::string& key, const std::vector<std::string>& values) {
            std::string config;
            config += key + YAML_CONFIG_KEY_DELIMITER;
            for(const auto& value : values) {
                config += YAML_CONFIG_DELIMITER + YAML_CONFIG_OPTION_CHARACTER + value;
            }
            config += YAML_CONFIG_DELIMITER;
            config += YAML_CONFIG_DELIMITER;
            return config;
        }

        std::string convertToConfig(std::string key, std::string value) {
            return std::string(key + YAML_CONFIG_KEY_DELIMITER + value) + YAML_CONFIG_DELIMITER + YAML_CONFIG_DELIMITER;
        }

        std::string convertToConfig(const std::string& key, const std::initializer_list<std::string>& values) {
            std::vector<std::string> vectorValues(values);
            return convertToConfig(key, vectorValues);
        }


        std::string basename(const std::string& file) {
            unsigned int found = file.find_last_of("/\\");
            return file.substr(0,found);
        }
    }
}

#endif  /* __TEST_UTILS_H__ */
