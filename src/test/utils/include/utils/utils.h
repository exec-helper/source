#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include <string>
#include <memory>
#include <vector>

#include "core/pattern.h"
#include "core/options.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }
    namespace core {
    }
}

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

            std::string convertToConfig(const config::SettingsNode& rootSettings, const std::vector<core::Pattern>& patterns = {}, const std::string& prepend = std::string()) noexcept;
            std::string convertToConfig(const std::string& key, const std::vector<std::string>& values, const std::string& prepend = std::string());
            std::string convertToConfig(std::string key, std::string value, const std::string& prepend = std::string());
            std::string convertToConfig(const std::string& key, const std::initializer_list<std::string>& values, const std::string& prepend = std::string());
            std::string basename(const std::string& file);

            config::SettingsNode& getSetting(config::SettingsNode& settings, const std::string& key) noexcept;
            void addSettings(config::SettingsNode& settings, const std::string& value) noexcept;
            void addSettings(config::SettingsNode& settings, const std::string& key, const std::string& value) noexcept;
            void addSettings(config::SettingsNode& settings, const std::string& key, const std::initializer_list<std::string>& values) noexcept;
            void addSettings(config::SettingsNode& settings, const std::string& key, const std::vector<std::string>& values) noexcept;
            void writeSettingsFile(const std::string& filename, const config::SettingsNode& settings, const std::vector<core::Pattern>& patterns) noexcept;

            core::PatternCombinations createPatternCombination(const std::initializer_list<core::PatternKey>& keys, const std::initializer_list<core::PatternValue>& values) noexcept;

            core::PatternCombinations createPatternCombination(const core::PatternKeys& keys, const core::PatternValues& values) noexcept;

            /**
             * Interface for defining pattern utils
             */ 
            struct PatternUtil {
                virtual std::vector<std::string> getKeys() const noexcept = 0;
                virtual std::vector<core::Pattern> getPatterns() const noexcept = 0;
                virtual core::PatternPermutator makePatternPermutator() const noexcept = 0;
                virtual core::PatternPermutator makePatternPermutator(const std::vector<core::Pattern>& patterns) const noexcept = 0;
            };

            struct TargetUtil : public PatternUtil {
                core::Pattern target;
                core::Pattern runTarget;

                TargetUtil();
                virtual std::vector<std::string> getKeys() const noexcept override;
                virtual std::vector<core::Pattern> getPatterns() const noexcept override;
                virtual core::PatternPermutator makePatternPermutator() const noexcept override;
                virtual core::PatternPermutator makePatternPermutator(const std::vector<core::Pattern>& patterns) const noexcept override;
            };

            struct CompilerUtil : public PatternUtil {
                core::Pattern compiler;
                core::Pattern mode;
                core::Pattern architecture;
                core::Pattern distribution;

                CompilerUtil();
                virtual std::vector<std::string> getKeys() const noexcept override;
                virtual std::vector<core::Pattern> getPatterns() const noexcept override;
                virtual core::PatternPermutator makePatternPermutator() const noexcept override;
                virtual core::PatternPermutator makePatternPermutator(const std::vector<core::Pattern>& patterns) const noexcept override;
            };

            typedef std::vector<core::Pattern> Patterns;
            core::PatternKeys getAllPatternKeys(const std::initializer_list<std::reference_wrapper<const PatternUtil>>& patterns) noexcept;
            Patterns getAllPatterns(const std::initializer_list<std::reference_wrapper<const PatternUtil>>& patterns) noexcept;
        }
    }
}

#endif  /* __TEST_UTILS_H__ */
