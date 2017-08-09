#include <string>
#include <utility>
#include <vector>

#include "unittest/catch.h"
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "config/settingsNode.h"
#include "core/task.h"
#include "plugins/pluginUtils.h"

#include "utils/utils.h"

using std::make_pair;
using std::string;
using std::vector;

using execHelper::config::SettingsNode;
using execHelper::core::TaskCollection;
using execHelper::core::PatternCombinations;
using execHelper::core::EnvironmentCollection;
using execHelper::core::EnvironmentValue;
using execHelper::test::utils::createPatternCombination;
using execHelper::test::utils::combineVectors;

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Test the patterns key", "[plugins][pluginUtils]") {
        GIVEN("The correct patterns key") {
            string correctPatternsKey("patterns");

            WHEN("We get the patterns key") {
                string actualPatternsKey = getPatternsKey();

                THEN("They should match") {
                    REQUIRE(actualPatternsKey == correctPatternsKey); 
                }
            }
        }
    }

    SCENARIO("Test the containing settings", "[plugins][pluginUtils]") {
        GIVEN("A key, a random command and a setting that is in the root settings tree") {
            const string command("random-command");
            const string key("random-key");
            SettingsNode rootSettings("pluginUtilsTest");
            rootSettings.add({key}, "random-value");

            WHEN("We request the containing settings") {
                boost::optional<const SettingsNode&> actualSettings = getContainingSettings(key, rootSettings, {});

                THEN("We should get the root settings") {
                    REQUIRE(actualSettings != boost::none);
                    REQUIRE(actualSettings.get() == rootSettings[key]);
                }
            }

            WHEN("We request the root settings") {
                boost::optional<const SettingsNode&> actualSettings = getContainingSettings(key, rootSettings, {command});

                THEN("We should get the root settings") {
                    REQUIRE(actualSettings == boost::none);
                }
            }
        }
        GIVEN("A settings config and a key that is not in the settings config") {
            const string command("random-command");
            const string key("random-key");
            const string keyNotInConfig("other-random-key");
            SettingsNode rootSettings("pluginUtilsTest");
            rootSettings.add({key}, "random-value");

            WHEN("We request the containing settings") {
                boost::optional<const SettingsNode&> actualSettings = getContainingSettings(key, rootSettings, {command});

                THEN("We should get the root settings") {
                    REQUIRE(actualSettings == boost::none);
                }
            }
        }

        GIVEN("A key, a random command and a setting that is in the command settings tree") {
            const string command("random-command");
            const string key("random-key");
            SettingsNode rootSettings("pluginUtilsTest");
            rootSettings.add({command, key}, "random-value");

            WHEN("We request the command settings") {
                boost::optional<const SettingsNode&> actualSettings = getContainingSettings(key, rootSettings, {command});

                THEN("We should get the command settings") {
                    REQUIRE(actualSettings != boost::none);
                    REQUIRE(actualSettings.get() == rootSettings[command][key]);
                }
            }

            WHEN("We request the root settings") {
                boost::optional<const SettingsNode&> actualSettings = getContainingSettings(key, rootSettings, {});

                THEN("We should get the command settings") {
                    REQUIRE(actualSettings == boost::none);
                }
            }
        }
    }

    SCENARIO("Test the command line option getter", "[plugins][pluginUtils]") {
        GIVEN("A settings configuration that does not contain a command line setting") {
            const string command("random-command");
            const string key("random-key");
            SettingsNode rootSettings("pluginUtilsTest");
            rootSettings.add({command, key}, "random-value");

            PatternCombinations combination = createPatternCombination({"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"}, {"compiler", "mode", "architecture", "distribution"});

            WHEN("We request the command line settings") {
                TaskCollection actualTaskCollection = getCommandLine(command, rootSettings, combination);

                THEN("We should get an empty response") {
                    REQUIRE(actualTaskCollection.empty());
                }
            }
        }
        GIVEN("A settings configuration that does contains an empty command line setting") {
            const string command("random-command");
            const string key("command-line");
            SettingsNode rootSettings("pluginUtilsTest");
            rootSettings.add({command}, key);

            PatternCombinations combination = createPatternCombination({"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"}, {"compiler", "mode", "architecture", "distribution"});

            WHEN("We request the command line settings") {
                TaskCollection actualTaskCollection = getCommandLine(command, rootSettings, combination);

                THEN("We should get an empty response") {
                    REQUIRE(actualTaskCollection.empty());
                }
            }
        }
        GIVEN("A settings configuration that does contains a command line setting") {
            const string command("random-command");
            const string key("command-line");
            const string value("random-command-line");
            SettingsNode rootSettings("pluginUtilsTest");
            rootSettings.add({command, key}, value);

            PatternCombinations combination = createPatternCombination({"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"}, {"compiler", "mode", "architecture", "distribution"});

            WHEN("We request the command line settings") {
                TaskCollection actualTaskCollection = getCommandLine(command, rootSettings, combination);

                THEN("We should get an empty response") {
                    TaskCollection correctTaskCollection({value});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
        }
        GIVEN("A settings configuration with a command that contains a specific command line setting and an other command that does not") {
            const string command("random-command");
            const string innerCommand1("inner-random-command1");
            const string innerCommand2("inner-random-command2");
            const string key("command-line");
            const string value1("random-command-line1");
            const string value2("random-command-line2");
            SettingsNode rootSettings(command);
            rootSettings.add({innerCommand1, key}, value1);
            rootSettings.add({command, innerCommand2}, "inner-command-command");
            rootSettings.add({key}, value2);

            PatternCombinations combination = createPatternCombination({"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"}, {"compiler", "mode", "architecture", "distribution"});

            WHEN("We request the command line settings of the outer command") {
                TaskCollection actualTaskCollection = getCommandLine(command, rootSettings, combination);

                THEN("We should get the outer response") {
                    TaskCollection correctTaskCollection({value2});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
            WHEN("We request the command line settings of the first inner command") {
                TaskCollection actualTaskCollection = getCommandLine(innerCommand1, rootSettings, combination);

                THEN("We should get the inner response") {
                    TaskCollection correctTaskCollection({value1});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
            WHEN("We request the command line settings of the second inner command") {
                TaskCollection actualTaskCollection = getCommandLine(innerCommand2, rootSettings, combination);

                THEN("We should get the general one") {
                    TaskCollection correctTaskCollection({value2});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
        }
        GIVEN("A settings configuration with a command that contains a specific command line setting and an other command that does not and a command-line with patterns") {
            const string command("random-command");
            const string innerCommand1("inner-random-command1");
            const string innerCommand2("inner-random-command2");
            const string key("command-line");
            const vector<string> value1({"build/{COMPILER}", "mode={MODE}", "hello"});
            const vector<string> value2({"other-build/{DISTRIBUTION}-2", "architecture={ARCHITECTURE}", "random-command-line2"});
            const vector<string> patterns({"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"});

            SettingsNode rootSettings(command);
            rootSettings.add({innerCommand1, key}, value1);
            rootSettings.add({command}, innerCommand2);
            rootSettings.add({innerCommand2}, "inner-command-command");
            rootSettings.add({key}, value2);
            rootSettings.add({"patterns"}, patterns);

            PatternCombinations combination = createPatternCombination({"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"}, {"compiler", "mode", "architecture", "distribution"});

            vector<string> replacedValue1({"build/compiler", "mode=mode", "hello"});
            vector<string> replacedValue2({"other-build/distribution-2", "architecture=architecture", "random-command-line2"});

            WHEN("We request the command line settings of the outer command") {
                TaskCollection actualTaskCollection = getCommandLine(command, rootSettings, combination);

                THEN("We should get the outer response") {
                    TaskCollection correctTaskCollection({replacedValue2});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
            WHEN("We request the command line settings of the first inner command") {
                TaskCollection actualTaskCollection = getCommandLine(innerCommand1, rootSettings, combination);

                THEN("We should get the inner response") {
                    TaskCollection correctTaskCollection({replacedValue1});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
            WHEN("We request the command line settings of the second inner command") {
                TaskCollection actualTaskCollection = getCommandLine(innerCommand2, rootSettings, combination);

                THEN("We should get the general one") {
                    TaskCollection correctTaskCollection({replacedValue2});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
        }
    }

    SCENARIO("Test the environment key", "[plugins][pluginUtils]") {
        GIVEN("The correct environment key") {
            string correctEnvironmentKey("environment");

            WHEN("We get the environment key") {
                string actualEnvironmentKey = getEnvironmentKey();

                THEN("They should match") {
                    REQUIRE(actualEnvironmentKey == correctEnvironmentKey); 
                }
            }
        }
    }

    SCENARIO("Test getting the environment", "[plugins][pluginUtils]") {
        MAKE_COMBINATIONS("Of several environment settings") {
            const string command("environment-command");
            const string environmentKey(getEnvironmentKey());

            SettingsNode rootSettings("plugin-key");
            const string otherCommandKey("other-command");

            SettingsNode::SettingsKeys baseSettingsKeys = {};
            SettingsNode::SettingsKeys otherBaseSettingsKeys = {otherCommandKey};
            const EnvironmentValue ENVIRONMENT_VALUE1 = make_pair("ENVIRONMENT_KEY1", "ENVIRONMENT_VALUE1");
            const EnvironmentValue ENVIRONMENT_VALUE2 = make_pair("ENVIRONMENT_KEY2", "ENVIRONMENT_VALUE2");
            const EnvironmentCollection ENVIRONMENT_VALUES({ENVIRONMENT_VALUE1, ENVIRONMENT_VALUE2});

            EnvironmentCollection actualEnvironment;

            COMBINATIONS("Switch between specific and general key") {
                baseSettingsKeys.push_back(command);
            }

            COMBINATIONS("Add one environment variable") {
                rootSettings.add(combineVectors(baseSettingsKeys, {environmentKey, ENVIRONMENT_VALUE1.first}), ENVIRONMENT_VALUE1.second);
                actualEnvironment.insert(ENVIRONMENT_VALUE1);
            }

            COMBINATIONS("Add environment variables") {
                for(const auto& environmentValue : ENVIRONMENT_VALUES) {
                    rootSettings.add(combineVectors(baseSettingsKeys, {environmentKey, environmentValue.first}), environmentValue.second);
                    actualEnvironment.insert(environmentValue);
                }
            }

            EnvironmentCollection returnedEnvironmentCollection = getEnvironment(command, rootSettings);

            THEN_CHECK("The returned environment collection is as expected") {
                REQUIRE(returnedEnvironmentCollection == actualEnvironment);
            }
        }
    }
} } }
