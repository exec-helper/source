#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include <string>
#include <memory>
#include <vector>

namespace execHelper {
    namespace test {
        namespace utils {
            template<typename T>
            void appendVectors(T& appendTo, const T& appendFrom) {
                appendTo.insert(std::end(appendTo), std::begin(appendFrom), std::end(appendFrom));
            }

            struct MainVariables {
                int argc;
                std::unique_ptr<char*[]> argv;

                MainVariables(const std::vector<std::string>& arguments);
            };

            std::string convertToConfig(const std::string& key, const std::vector<std::string>& values);
            std::string convertToConfig(std::string key, std::string value);
            std::string convertToConfig(const std::string& key, const std::initializer_list<std::string>& values);
            std::string basename(const std::string& file);
        }
    }
}

#endif  /* __TEST_UTILS_H__ */
