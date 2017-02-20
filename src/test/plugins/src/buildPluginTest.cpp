#include <vector>
#include <string>

#include <catch.hpp>

#include "config/settingsNode.h"
#include "core/task.h"
#include "plugins/pluginUtils.h"
#include "plugins/buildPlugin.h"
#include "utils/utils.h"

#include "optionsStub.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::TaskCollection;
using execHelper::core::PatternCombinations;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::createPatternCombination;
using execHelper::test::OptionsStub;

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Test build dir key getter", "[plugins][buildPlugin]") {
        GIVEN("Nothing in particular") {
            WHEN("We get the build dir key") {
                const string buildDirKey = BuildPlugin::getBuildDirKey();

                THEN("We should find the appropriate one") {
                    REQUIRE(buildDirKey == "build-dir");
                }
            }
        }
    }

    SCENARIO("Test the multi threaded setting", "[plugins][buildPlugin]") {
        string command("random-command");
        string key("single-threaded");

        SettingsNode rootSettings;
        OptionsStub options;

        GIVEN("A default configuration setting") {
            bool correctResult = true;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                }
            }
        }
        GIVEN("A default configuration setting with an empty value for the multi-threaded setting") {
            addSettings(rootSettings, command, key);
            bool correctResult = true;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                }
            }
        }
        GIVEN("A default configuration setting with the multi-threaded setting set") {
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "no");
            bool correctResult = true;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                }
            }
        }
        GIVEN("A default configuration setting with the single-threaded setting set") {
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "yes");
            bool correctResult = false;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                }
            }
        }
        GIVEN("A default configuration setting with the single-threaded setting set on different levels") {
            string subCommand("sub-command");
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "no");
            addSettings(rootSettings[command], subCommand, key);
            addSettings(rootSettings[command][subCommand], key, "yes");
            bool correctResult = true;
            bool correctSubResult = false;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 
                bool actualSubResult = BuildPlugin::getMultiThreaded(subCommand, rootSettings[command], options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                    REQUIRE(actualSubResult == correctSubResult);
                }
            }
        }
        GIVEN("A default configuration setting with the single-threaded option set") {
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "no");
            bool correctResult = false;

            options.m_singleThreaded = true;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                }
            }
        }
    }
} } }
