#include "unittest/catch.h"

#include <boost/lexical_cast.hpp>
#include <thread>

#include "config/commandLineOptions.h"
#include "config/fleetingOptions.h"
#include "config/variablesMap.h"
#include "log/log.h"
#include "log/logLevel.h"

using std::thread;
using std::to_string;

using boost::lexical_cast;

using execHelper::log::LogLevel;
using execHelper::log::all;
using execHelper::log::toLogLevel;

namespace execHelper {
namespace config {
namespace test {
SCENARIO("Test the fleeting options defaults", "[config][fleeting-options]") {
    execHelper::log::init();

    GIVEN("The expected defaults") {
        VariablesMap expectedDefaults("exec-helper");
        expectedDefaults.add(HELP_KEY, "no");
        expectedDefaults.add(VERBOSE_KEY, "no");
        expectedDefaults.add(JOBS_KEY, "auto");
        expectedDefaults.add(DRY_RUN_KEY, "no");
        expectedDefaults.add(LOG_LEVEL_KEY, "none");
        expectedDefaults.add(COMMAND_KEY);
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
    MAKE_COMBINATIONS("Of several settings") {
        VariablesMap variables = FleetingOptions::getDefault();

        auto expectedHelp = variables.get<HelpOption_t>(HELP_KEY).get();
        auto expectedVerbose =
            variables.get<VerboseOption_t>(VERBOSE_KEY).get();
        auto jobs = variables.get<JobsOption_t>(JOBS_KEY).get();
        auto expectedJobs = 0U;
        if(jobs == "auto") {
            expectedJobs = thread::hardware_concurrency();
        } else {
            expectedJobs = lexical_cast<Jobs_t>(jobs);
        }
        auto expectedDryRun = variables.get<DryRunOption_t>(DRY_RUN_KEY).get();
        auto expectedLogLevel =
            toLogLevel(variables.get<LogLevelOption_t>(LOG_LEVEL_KEY).get());
        auto expectedCommands =
            variables.get<CommandCollection>(COMMAND_KEY).get();

        COMBINATIONS("Enable help") {
            expectedHelp = true;
            variables.add(HELP_KEY, "yes");
        }

        COMBINATIONS("Enable verbosity") {
            expectedVerbose = true;
            variables.add(VERBOSE_KEY, "yes");
        }

        COMBINATIONS("Enable single threaded") {
            expectedJobs = 1U;
            variables.add(JOBS_KEY, to_string(expectedJobs));
        }

        COMBINATIONS("Enable number of jobs") {
            expectedJobs = 6U;
            variables.replace(JOBS_KEY, to_string(expectedJobs));
        }

        COMBINATIONS("Enable dry run") {
            expectedDryRun = true;
            variables.add(DRY_RUN_KEY, "yes");
        }

        COMBINATIONS("Disable logging") {
            expectedLogLevel = all;
            variables.add(LOG_LEVEL_KEY, "all");
        }

        COMBINATIONS("Add commands") {
            expectedCommands.emplace_back("command1");
            expectedCommands.emplace_back("command2");
        }

        variables.add(COMMAND_KEY, expectedCommands);

        THEN_WHEN("We create fleeting options based on the variables map") {
            FleetingOptions fleetingOptions(variables);

            THEN_CHECK("The getters are as expected") {
                REQUIRE(fleetingOptions.getHelp() == expectedHelp);
                REQUIRE(fleetingOptions.getVerbosity() == expectedVerbose);
                REQUIRE(fleetingOptions.getJobs() == expectedJobs);
                REQUIRE(fleetingOptions.getDryRun() == expectedDryRun);
                REQUIRE(fleetingOptions.getLogLevel() == expectedLogLevel);
                REQUIRE(fleetingOptions.getCommands() == expectedCommands);
            }
        }
    }
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
} // namespace test
} // namespace config
} // namespace execHelper
