#include <locale>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/replace.hpp>

#include "unittest/catch.h"

#include "config/environment.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/pluginUtils.h"

#include "base-utils/nonEmptyString.h"
#include "log/generators.h"
#include "unittest/rapidcheck.h"
#include "utils/utils.h"

using std::make_pair;
using std::string;

using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::EnvironmentCollection;
using execHelper::config::EnvironmentValue;
using execHelper::config::PatternCombinations;
using execHelper::config::VariablesMap;
using execHelper::core::TaskCollection;

using execHelper::test::NonEmptyString;
using execHelper::test::propertyTest;

using NonEmptyPatternCombinations = std::map<NonEmptyString, std::string>;

namespace {
inline auto toReplacementPattern(const std::string& pattern) noexcept
    -> std::string {
    return std::string("{").append(pattern).append("}");
}
} // namespace

namespace execHelper::plugins::test {
SCENARIO("Test the patterns key", "[plugin-utils]") {
    GIVEN("The correct patterns key") {
        string correctPatternsKey("patterns");

        WHEN("We get the patterns key") {
            const string& actualPatternsKey = getPatternsKey();

            THEN("They should match") {
                REQUIRE(actualPatternsKey == correctPatternsKey);
            }
        }
    }
}

SCENARIO("Test getting the environment", "[plugin-utils]") {
    GIVEN("Some configured environment variables") {
        const EnvironmentValue ENVIRONMENT_VALUE1 =
            make_pair("ENVIRONMENT_KEY1", "ENVIRONMENT_VALUE1");
        const EnvironmentValue ENVIRONMENT_VALUE2 =
            make_pair("ENVIRONMENT_KEY2", "ENVIRONMENT_VALUE2");
        const EnvironmentCollection ENVIRONMENT_VALUES(
            {ENVIRONMENT_VALUE1, ENVIRONMENT_VALUE2});

        VariablesMap variables("test");
        for(const auto& env : ENVIRONMENT_VALUES) {
            REQUIRE(variables.add({ENVIRONMENT_KEY, env.first}, env.second));
        }

        WHEN("We get the environment") {
            const EnvironmentCollection result = getEnvironment(variables);

            THEN("We should find the right environment") {
                REQUIRE(ENVIRONMENT_VALUES == result);
            }
        }
    }
}

SCENARIO("Test the working directory key", "[plugin-utils]") {
    GIVEN("The correct working directory key") {
        string correctWorkingDirKey("working-dir");

        WHEN("We get the working directory key") {
            const string& actualWorkingDirKey = getWorkingDirKey();

            THEN("They should match") {
                REQUIRE(actualWorkingDirKey == correctWorkingDirKey);
            }
        }
    }
}

SCENARIO("Test replacing patterns in the environment", "[plugin-utils]") {
    propertyTest(
        "Replace a pattern in the environment",
        [](const NonEmptyPatternCombinations& nonEmptyPatternCombinations,
           const EnvironmentCollection& startEnvironment) {
            // Replace '{' and '}' with '%'
            PatternCombinations patternCombinations;
            std::transform(
                nonEmptyPatternCombinations.begin(),
                nonEmptyPatternCombinations.end(),
                std::inserter(patternCombinations, patternCombinations.end()),
                [](const auto& combination) {
                    auto key = boost::algorithm::replace_all_copy(
                        *(combination.first), "{", "%");
                    boost::algorithm::replace_all(key, "}", "%");

                    auto value = boost::algorithm::replace_all_copy(
                        combination.second, "{", "%");
                    boost::algorithm::replace_all(value, "}", "%");
                    return make_pair(key, value);
                });

            // Replace '{' and '}' with '%'
            EnvironmentCollection replacedEnvironment;
            std::transform(
                startEnvironment.begin(), startEnvironment.end(),
                std::inserter(replacedEnvironment, replacedEnvironment.end()),
                [](const auto& env) {
                    auto key =
                        boost::algorithm::replace_all_copy(env.first, "{", "%");
                    boost::algorithm::replace_all(key, "}", "%");

                    auto value = boost::algorithm::replace_all_copy(env.second,
                                                                    "{", "%");
                    boost::algorithm::replace_all(value, "}", "%");
                    return make_pair(key, value);
                });

            EnvironmentCollection inputEnvironment(replacedEnvironment);
            EnvironmentCollection expected(replacedEnvironment);

            for(const auto& combination : patternCombinations) {
                auto replacementPattern =
                    toReplacementPattern(combination.first);
                inputEnvironment.emplace(
                    make_pair("key-" + replacementPattern,
                              "value-" + replacementPattern));
                expected.emplace(make_pair("key-" + combination.second,
                                           "value-" + combination.second));
            }

            THEN_WHEN("We replace the patterns") {
                auto actual = replacePatternsInEnvironment(inputEnvironment,
                                                           patternCombinations);

                THEN_CHECK("The actual task equals the expected task") {
                    if(actual != expected) {
                        std::for_each(patternCombinations.begin(),
                                      patternCombinations.end(),
                                      [](const auto& combination) {
                                          std::cout << combination.first
                                                    << std::endl;
                                      });
                    }
                    REQUIRE(actual == expected);
                }
            }
        });
}
} // namespace execHelper::plugins::test
