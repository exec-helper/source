#include "unittest/catch.h"

#include <algorithm>
#include <string>

#include "config/commandLineOptions.h"
#include "config/fleetingOptions.h"
#include "config/variablesMap.h"
#include "log/logLevel.h"

#include "config/generators.h"
#include "log/generators.h"
#include "unittest/rapidcheck.h"
#include "utils/testValue.h"

using std::back_inserter;
using std::string;
using std::transform;

using execHelper::log::LogLevel;
using execHelper::log::toString;

using execHelper::test::propertyTest;

using rc::DryRunValue;
using rc::HelpValue;
using rc::JobsValue;
using rc::ListPluginsValue;
using rc::VerbosityValue;
using rc::VersionValue;
using rc::AppendSearchPathValue;

namespace  {
    inline std::vector<std::string> toStrings(const execHelper::config::Paths& paths) noexcept {
        std::vector<std::string> strings;
        transform(paths.begin(), paths.end(), back_inserter(strings), [](const auto& path) {
            return path.string();
        });
        return strings;
    }
} // namespace 

namespace execHelper::config::test {
SCENARIO("Test the fleeting options defaults", "[config][fleeting-options]") {
    GIVEN("The expected defaults") {
        VariablesMap expectedDefaults("exec-helper");
        REQUIRE(expectedDefaults.add(HELP_OPTION_KEY, "no"));
        REQUIRE(expectedDefaults.add(VERSION_KEY, "no"));
        REQUIRE(expectedDefaults.add(VERBOSE_KEY, "no"));
        REQUIRE(expectedDefaults.add(JOBS_KEY, "auto"));
        REQUIRE(expectedDefaults.add(DRY_RUN_KEY, "no"));
        REQUIRE(expectedDefaults.add(LOG_LEVEL_KEY, "none"));
        REQUIRE(expectedDefaults.add(LIST_PLUGINS_KEY, "no"));
        REQUIRE(expectedDefaults.add(APPEND_SEARCH_PATH_KEY, AppendSearchPathOption_t()));
        REQUIRE(expectedDefaults.add(COMMAND_KEY, CommandCollection()));
        REQUIRE(expectedDefaults.add(SETTINGS_FILE_KEY));

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
           const ListPluginsValue& listPlugins, const AppendSearchPathValue& appendedSearchPaths,
           const CommandCollection& commands) {
            VariablesMap variables = FleetingOptions::getDefault();

            REQUIRE(variables.replace(HELP_OPTION_KEY, help.config()));
            REQUIRE(variables.replace(VERSION_KEY, version.config()));
            REQUIRE(variables.replace(VERBOSE_KEY, verbosity.config()));
            REQUIRE(variables.replace(JOBS_KEY, jobs.config()));
            REQUIRE(variables.replace(DRY_RUN_KEY, dryRun.config()));
            REQUIRE(
                variables.replace(LOG_LEVEL_KEY, string{toString(logLevel)}));
            REQUIRE(variables.replace(LIST_PLUGINS_KEY, listPlugins.config()));
            REQUIRE(variables.replace(APPEND_SEARCH_PATH_KEY, toStrings(appendedSearchPaths)));
            REQUIRE(variables.replace(COMMAND_KEY, commands));

            THEN_WHEN("We create fleeting options based on the variables map") {
                FleetingOptions fleetingOptions(variables);

                THEN_CHECK("The getters are as expected") {
                    REQUIRE(help == fleetingOptions.getHelp());
                    REQUIRE(version == fleetingOptions.getVersion());
                    REQUIRE(verbosity == fleetingOptions.getVerbosity());
                    REQUIRE(jobs == fleetingOptions.getJobs());
                    REQUIRE(dryRun == fleetingOptions.getDryRun());
                    REQUIRE(logLevel == fleetingOptions.getLogLevel());
                    REQUIRE(listPlugins == fleetingOptions.listPlugins());
                    REQUIRE(appendedSearchPaths == fleetingOptions.appendedSearchPaths());
                    REQUIRE(commands == fleetingOptions.getCommands());
                }
            }
        });
}

SCENARIO("Test copy constructor and assignment", "[patterns][Pattern]") {
    GIVEN("An object to copy") {
        VariablesMap variables = FleetingOptions::getDefault();
        REQUIRE(variables.replace(COMMAND_KEY, "command"));
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
