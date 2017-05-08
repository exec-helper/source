#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#define THEN_WHEN(x)
#define THEN_CHECK(x)

#include <initializer_list>
#include <string>
#include <memory>
#include <vector>

#include "core/pattern.h"
#include "core/options.h"

#include "optionsStub.h"
#include "executorStub.h"

namespace execHelper {
    namespace config {
        class SettingsNode;
    }
}

namespace execHelper {
    namespace test {
        namespace utils {
            typedef std::vector<core::Pattern> Patterns;

            template<typename T>
            void appendVectors(T& appendTo, const T& appendFrom) {
                appendTo.insert(std::end(appendTo), std::begin(appendFrom), std::end(appendFrom));
            }

            template<typename T>
            T combineVectors(const T& append1, const T& append2) {
                T result = append1;
                appendVectors(result, append2);
                return result;
            }

            template<typename T, typename U>
            T copyAndAppend(const T& append1, U&& newElement) {
                T result = append1;
                result.emplace_back(newElement);
                return result;
            }

            struct MainVariables {
                int argc;
                std::unique_ptr<char*[]> argv;

                MainVariables(const std::vector<std::string>& arguments);
            };

            std::string convertToConfig(const Patterns& patterns) noexcept;
            std::string convertToConfig(const config::SettingsNode& rootSettings, const std::string& prepend = std::string()) noexcept;
            std::string convertToConfig(const config::SettingsNode& settings, const Patterns& patterns, const std::string& prepend = std::string()) noexcept;

            std::string convertToConfig(std::string key, std::string value, const std::string& prepend = std::string());
            std::string convertToConfig(const std::string& key, const std::initializer_list<std::string>& values, const std::string& prepend = std::string());
            std::string convertToConfig(const std::string& key, const std::vector<std::string>& values, const std::string& prepend = std::string());

            std::string convertToConfig(const std::initializer_list<std::string>& keys, const std::string& value, const std::string& prepend = std::string()) noexcept;
            std::string convertToConfig(const std::initializer_list<std::string>& keys, const std::initializer_list<std::string>& values, const std::string& prepend = std::string()) noexcept;
            std::string convertToConfig(const std::initializer_list<std::string>& keys, const std::vector<std::string>& values, const std::string& prepend = std::string()) noexcept;
            std::string convertToConfig(const std::vector<std::string>& keys, const std::initializer_list<std::string>& values, const std::string& prepend = std::string()) noexcept;
            std::string convertToConfig(const std::vector<std::string>& keys, const std::vector<std::string>& values, const std::string& prepend = std::string()) noexcept;
            std::string basename(const std::string& file);

            void addSettings(config::SettingsNode& settings, const config::SettingsNode::SettingsKey& key, const config::SettingsNode::SettingsValue& value) noexcept;
            void addSettings(config::SettingsNode& settings, const config::SettingsNode::SettingsKey& key, const std::initializer_list<std::string>& values) noexcept;
            void addSettings(config::SettingsNode& settings, const config::SettingsNode::SettingsKey& key, const config::SettingsNode::SettingsValues& values) noexcept;
            void addSettings(config::SettingsNode& settings, const config::SettingsNode::SettingsKeys& key, const config::SettingsNode::SettingsValues& values) noexcept;
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

            struct TargetUtilNames {
                std::string target;
                std::string runTarget;
            };

            struct TargetUtil : public PatternUtil {
                core::Pattern target;
                core::Pattern runTarget;

                TargetUtil();
                virtual std::vector<std::string> getKeys() const noexcept override;
                virtual std::vector<core::Pattern> getPatterns() const noexcept override;
                virtual core::PatternPermutator makePatternPermutator() const noexcept override;
                virtual core::PatternPermutator makePatternPermutator(const std::vector<core::Pattern>& patterns) const noexcept override;

                TargetUtilNames toNames(const std::map<core::PatternKey, core::PatternValue>& pattern) const noexcept;
            };

            struct CompilerUtilNames {
                std::string compiler;
                std::string mode;
                std::string architecture;
                std::string distribution;
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
                
                CompilerUtilNames toNames(const std::map<core::PatternKey, core::PatternValue>& pattern) const noexcept;
            };

            core::PatternKeys getAllPatternKeys(const std::initializer_list<std::reference_wrapper<const PatternUtil>>& patterns) noexcept;
            Patterns getAllPatterns(const std::initializer_list<std::reference_wrapper<const PatternUtil>>& patterns) noexcept;

            void addPatterns(const Patterns& patterns, OptionsStub& options) noexcept;

            const core::Pattern PATTERN1("PATTERN1", {"pattern1"}, 'p', "--pattern1");
            const core::Pattern PATTERN2("PATTERN2", {"pattern2a", "pattern2b"}, 'q', "--pattern2");
            const Patterns PATTERNS({PATTERN1, PATTERN2});
            const core::PatternKeys PATTERN_KEYS({PATTERN1.getKey(), PATTERN2.getKey()});

            core::test::ExecutorStub::TaskQueue getExpectedTasks(const core::Task& expectedTask, const TargetUtil& targetUtil) noexcept;
            core::test::ExecutorStub::TaskQueue getExpectedTasks(const core::Task& expectedTask, const CompilerUtil& compilerUtil, const TargetUtil& targetUtil) noexcept;
            core::test::ExecutorStub::TaskQueue getExpectedTasks(const core::test::ExecutorStub::TaskQueue& expectedTask, const CompilerUtil& compilerUtil, const TargetUtil& targetUtil) noexcept;

            std::string toString(const config::SettingsNode& settings, unsigned int nbOfTabs = 0) noexcept;
            bool fileExists(const std::string& path) noexcept;
        }
    }
}

#endif  /* __TEST_UTILS_H__ */
