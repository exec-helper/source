#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "config/settingsNode.h"
#include "core/task.h"
#include "plugins/pluginUtils.h"

#include "utils/utils.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::TaskCollection;
using execHelper::core::PatternCombinations;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::createPatternCombination;

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
            string command("random-command");
            string key("random-key");
            SettingsNode rootSettings;
            addSettings(rootSettings, key, "random-value");

            WHEN("We request the containing settings") {
                boost::optional<const SettingsNode&> actualSettings = getContainingSettings(key, rootSettings, {command});

                THEN("We should get the root settings") {
                    REQUIRE(actualSettings != boost::none);
                    REQUIRE(actualSettings.get() == rootSettings);
                }
            }
        }
        GIVEN("A settings config and a key that is not in the settings config") {
            string command("random-command");
            string key("random-key");
            string keyNotInConfig("other-random-key");
            SettingsNode rootSettings;
            addSettings(rootSettings, key, "random-value");

            WHEN("We request the containing settings") {
                boost::optional<const SettingsNode&> actualSettings = getContainingSettings(key, rootSettings, {command});

                THEN("We should get the root settings") {
                    REQUIRE(actualSettings != boost::none);
                    REQUIRE(actualSettings.get() == rootSettings);
                }
            }
        }

        GIVEN("A key, a random command and a setting that is in the command settings tree") {
            string command("random-command");
            string key("random-key");
            SettingsNode rootSettings;
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "random-value");

            WHEN("We request the containing settings") {
                boost::optional<const SettingsNode&> actualSettings = getContainingSettings(key, rootSettings, {command});

                THEN("We should get the command settings") {
                    REQUIRE(actualSettings != boost::none);
                    REQUIRE(actualSettings.get() == rootSettings[command]);
                }
            }
        }
    }

    SCENARIO("Test the command line option getter", "[plugins][pluginUtils]") {
        GIVEN("A settings configuration that does not contain a command line setting") {
            string command("random-command");
            string key("random-key");
            SettingsNode rootSettings;
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "random-value");

            PatternCombinations combination = createPatternCombination({"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"}, {"compiler", "mode", "architecture", "distribution"});

            WHEN("We request the command line settings") {
                TaskCollection actualTaskCollection = getCommandLine(command, rootSettings, combination);

                THEN("We should get an empty response") {
                    REQUIRE(actualTaskCollection.empty() == true);
                }
            }
        }
        GIVEN("A settings configuration that does contains an empty command line setting") {
            string command("random-command");
            string key("command-line");
            SettingsNode rootSettings;
            addSettings(rootSettings, command, key);

            PatternCombinations combination = createPatternCombination({"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"}, {"compiler", "mode", "architecture", "distribution"});

            WHEN("We request the command line settings") {
                TaskCollection actualTaskCollection = getCommandLine(command, rootSettings, combination);

                THEN("We should get an empty response") {
                    REQUIRE(actualTaskCollection.empty() == true);
                }
            }
        }
        GIVEN("A settings configuration that does contains a command line setting") {
            string command("random-command");
            string key("command-line");
            string value("random-command-line");
            SettingsNode rootSettings;
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, value);

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
            string command("random-command");
            string innerCommand1("inner-random-command1");
            string innerCommand2("inner-random-command2");
            string key("command-line");
            string value1("random-command-line1");
            string value2("random-command-line2");
            SettingsNode rootSettings;
            rootSettings.m_key = command;
            addSettings(rootSettings, innerCommand1, key);
            addSettings(rootSettings[innerCommand1], key, value1);
            addSettings(rootSettings, command, innerCommand2);
            addSettings(rootSettings, innerCommand2, "inner-command-command");
            addSettings(rootSettings, key, value2);

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
            string command("random-command");
            string innerCommand1("inner-random-command1");
            string innerCommand2("inner-random-command2");
            string key("command-line");
            vector<string> value1({"build/{COMPILER}", "mode={MODE}", "hello"});
            vector<string> value2({"other-build/{DISTRIBUTION}-2", "architecture={ARCHITECTURE}", "random-command-line2"});

            vector<string> patterns({"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"});

            SettingsNode rootSettings;
            rootSettings.m_key = command;
            addSettings(rootSettings, innerCommand1, key);
            addSettings(rootSettings[innerCommand1], key, value1);
            addSettings(rootSettings, command, innerCommand2);
            addSettings(rootSettings, innerCommand2, "inner-command-command");
            addSettings(rootSettings, key, value2);
            addSettings(rootSettings, "patterns", patterns);

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
} } }
