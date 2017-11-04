#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#define THEN_WHEN(x)
#define THEN_CHECK(x)

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include <gsl/gsl>
#include <gsl/string_span>

#include "base-utils/yaml.h"
#include "config/path.h"
#include "config/pattern.h"
#include "plugins/pluginUtils.h"

#include "executorStub.h"

namespace execHelper {
namespace config {
class SettingsNode;
} // namespace config

namespace test {
namespace baseUtils {
class ConfigFileWriter;
} // namespace baseUtils
} // namespace test
} // namespace execHelper

// Hack required for streaming boost::optional<vector<T>> types
namespace std {
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& stream) {
    for(const auto& element : stream) {
        os << element << ", ";
    }
    return os;
}

template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& stream) {
    os << stream.first << ": " << stream.second;
    return os;
}
} // namespace std

namespace execHelper {
namespace test {
namespace utils {
using Patterns = std::vector<config::Pattern>;

using Arguments = std::vector<std::string>;
struct MainVariables {
    int argc;
    std::unique_ptr<char* []> argv;

    explicit MainVariables(const Arguments& arguments);
};

template <typename T> void appendVectors(T& appendTo, const T& appendFrom) {
    appendTo.insert(std::end(appendTo), std::begin(appendFrom),
                    std::end(appendFrom));
}

baseUtils::YamlWriter toYaml(const config::SettingsNode& settings,
                             const config::Patterns& patterns) noexcept;
void writeSettingsFile(
    gsl::not_null<baseUtils::ConfigFileWriter*> configFileWriter,
    const config::SettingsNode& settings,
    const config::Patterns& patterns) noexcept;
std::string convertToConfig(const Patterns& patterns) noexcept;
std::string
convertToConfig(const config::SettingsNode& rootSettings,
                const std::string& prepend = std::string()) noexcept;
std::string
convertToConfig(const config::SettingsNode& settings, const Patterns& patterns,
                const std::string& prepend = std::string()) noexcept;

std::string convertToConfig(std::string key, std::string value,
                            const std::string& prepend = std::string());
std::string convertToConfig(const std::string& key,
                            const std::initializer_list<std::string>& values,
                            const std::string& prepend = std::string());
std::string convertToConfig(const std::string& key,
                            const std::vector<std::string>& values,
                            const std::string& prepend = std::string());

std::string
convertToConfig(const std::initializer_list<std::string>& keys,
                const std::string& value,
                const std::string& prepend = std::string()) noexcept;
std::string
convertToConfig(const std::initializer_list<std::string>& keys,
                const std::initializer_list<std::string>& values,
                const std::string& prepend = std::string()) noexcept;
std::string
convertToConfig(const std::initializer_list<std::string>& keys,
                const std::vector<std::string>& values,
                const std::string& prepend = std::string()) noexcept;
std::string
convertToConfig(const std::vector<std::string>& keys,
                const std::initializer_list<std::string>& values,
                const std::string& prepend = std::string()) noexcept;
std::string
convertToConfig(const std::vector<std::string>& keys,
                const std::vector<std::string>& values,
                const std::string& prepend = std::string()) noexcept;
std::string basename(const std::string& file);

config::PatternCombinations createPatternCombination(
    const std::initializer_list<config::PatternKey>& keys,
    const std::initializer_list<config::PatternValue>& values) noexcept;

config::PatternCombinations
createPatternCombination(const config::PatternKeys& keys,
                         const config::PatternValues& values) noexcept;
plugins::PatternPermutator
makePatternPermutator(const config::Patterns& patterns) noexcept;
core::test::ExecutorStub::TaskQueue
getExpectedTasks(const core::Task& task,
                 const config::Patterns patterns) noexcept;
core::test::ExecutorStub::TaskQueue
getExpectedTasks(const core::test::ExecutorStub::TaskQueue& tasks,
                 const config::Patterns patterns) noexcept;

std::string toString(const config::SettingsNode& settings,
                     unsigned int nbOfTabs = 0) noexcept;
} // namespace utils
} // namespace test
} // namespace execHelper

#endif /* __TEST_UTILS_H__ */
