#include "unittest/catch.h"

#include "config/commandLineOptions.h"
#include "config/fleetingOptions.h"
#include "config/variablesMap.h"
#include "log/logLevel.h"

#include "config/generators.h"
#include "log/generators.h"
#include "unittest/rapidcheck.h"
#include "utils/testValue.h"

using execHelper::log::LogLevel;
using execHelper::log::toString;

using execHelper::test::propertyTest;

using rc::DryRunValue;
using rc::HelpValue;
using rc::JobsValue;
using rc::VerbosityValue;
using rc::VersionValue;

namespace execHelper::config::test {
SCENARIO("Test the fleeting options defaults", "[config][fleeting-options]") {
    GIVEN("The expected defaults") {
        VariablesMap expectedDefaults("exec-helper");
        expectedDefaults.add(HELP_OPTION_KEY, "no");
        expectedDefaults.add(VERSION_KEY, "no");
        expectedDefaults.add(VERBOSE_KEY, "no");
        expectedDefaults.add(JOBS_KEY, "auto");
        expectedDefaults.add(DRY_RUN_KEY, "no");
        expectedDefaults.add(LOG_LEVEL_KEY, "none");
        expectedDefaults.add(COMMAND_KEY, CommandCollection());
        expectedDefaults.add(SETTINGS_FILE_KEY);

        WHEN("We request the defaults") {
            VariablesMap defaults = FleetingOptions::getDefault();

            THEN("They must match the expected ones") {
                REQUIRE(defaults == expectedDefaults);
            }
        }
    }
}

SCENARIO("Test the getters of the fleeting options",
         "[config][fleeting-options]") {
    propertyTest(
        "Test multiple configurations",
        [](const HelpValue& help, const VersionValue& version,
           const VerbosityValue& verbosity, const JobsValue& jobs,
           const DryRunValue& dryRun, const LogLevel& logLevel,
           const CommandCollection& commands) {
            VariablesMap variables = FleetingOptions::getDefault();

            variables.replace(HELP_OPTION_KEY, help.config());
            variables.replace(VERSION_KEY, version.config());
            variables.replace(VERBOSE_KEY, verbosity.config());
            variables.replace(JOBS_KEY, jobs.config());
            variables.replace(DRY_RUN_KEY, dryRun.config());
            variables.replace(LOG_LEVEL_KEY, toString(logLevel));
            variables.replace(COMMAND_KEY, commands);

            THEN_WHEN("We create fleeting options based on the variables map") {
                FleetingOptions fleetingOptions(variables);

                THEN_CHECK("The getters are as expected") {
                    REQUIRE(help == fleetingOptions.getHelp());
                    REQUIRE(version == fleetingOptions.getVersion());
                    REQUIRE(verbosity == fleetingOptions.getVerbosity());
                    REQUIRE(jobs == fleetingOptions.getJobs());
                    REQUIRE(dryRun == fleetingOptions.getDryRun());
                    REQUIRE(logLevel == fleetingOptions.getLogLevel());
                    REQUIRE(commands == fleetingOptions.getCommands());
                }
            }
        });
}

SCENARIO("Test copy constructor and assignment", "[patterns][Pattern]") {
    GIVEN("An object to copy") {
        VariablesMap variables = FleetingOptions::getDefault();
        variables.replace(COMMAND_KEY, "command");
        FleetingOptions originalFleetingOptions(variables);

        WHEN("We copy the fleeting options") {
            FleetingOptions
                copied( // NOLINT(performance-unnecessary-copy-initialization)
                    originalFleetingOptions);

            THEN("We should get the same fleeting options") {
                REQUIRE(originalFleetingOptions == copied);
                REQUIRE_FALSE(originalFleetingOptions != copied);
            }
        }

        WHEN("We assign the pattern") {
            FleetingOptions
                copied = // NOLINT(performance-unnecessary-copy-initialization)
                originalFleetingOptions;

            THEN("We should get the same pattern") {
                REQUIRE(originalFleetingOptions == copied);
                REQUIRE_FALSE(originalFleetingOptions != copied);
            }
        }
    }
}
} // namespace execHelper::config::test
