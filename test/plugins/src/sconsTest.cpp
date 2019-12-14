#include <string>

#include <gsl/string_span>

#include "config/path.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "plugins/commandLine.h"
#include "plugins/pluginUtils.h"
#include "plugins/scons.h"
#include "plugins/threadedness.h"
#include "plugins/verbosity.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::string;
using std::to_string;

using gsl::czstring;

using execHelper::config::Path;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::utils::getExpectedTasks;

namespace {
const czstring<> PLUGIN_NAME = "scons";
const czstring<> BUILD_DIR_KEY = "build-dir";
} // namespace

namespace execHelper::plugins::test {
SCENARIO("Obtain the plugin name of the scons plugin", "[scons]") {
    GIVEN("A plugin") {
        Scons plugin;

        WHEN("We request the plugin name") {
            const string pluginName = plugin.getPluginName();

            THEN("We should find the correct plugin name") {
                REQUIRE(pluginName == PLUGIN_NAME);
            }
        }
    }
}

SCENARIO("Obtain the default variables map of the scons plugin", "[scons]") {
    MAKE_COMBINATIONS("The default fleeting options") {
        FleetingOptionsStub fleetingOptions;
        Scons plugin;

        VariablesMap actualVariables(plugin.getPluginName());
        REQUIRE(actualVariables.add(BUILD_DIR_KEY, "."));
        REQUIRE(actualVariables.add(COMMAND_LINE_KEY, CommandLineArgs()));
        REQUIRE(actualVariables.add(VERBOSITY_KEY, "no"));
        REQUIRE(actualVariables.add(JOBS_KEY,
                                    to_string(fleetingOptions.getJobs())));

        COMBINATIONS("Switch on verbosity") {
            fleetingOptions.m_verbose = true;
            REQUIRE(actualVariables.replace(VERBOSITY_KEY, "yes"));
        }

        COMBINATIONS("Switch on single threaded") {
            fleetingOptions.m_jobs = 1U;
            REQUIRE(actualVariables.replace(JOBS_KEY,
                                            to_string(fleetingOptions.m_jobs)));
        }

        THEN_WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN_CHECK("We should find the same ones") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO("Testing the configuration settings of the scons plugin", "[scons]") {
    MAKE_COMBINATIONS("Of several settings") {
        const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
        const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
        const Patterns patterns({pattern1, pattern2});

        Scons plugin;
        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        Path workingDir(".");
        CommandLineArgs commandLine;
        bool verbosity = false;
        auto jobs = variables.get<Jobs>(JOBS_KEY).value();

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        COMBINATIONS("Switch off threading") {
            jobs = 1U;
            REQUIRE(variables.replace(JOBS_KEY, to_string(jobs)));
        }

        COMBINATIONS("Switch on verbosity") {
            verbosity = true;
            REQUIRE(variables.replace(VERBOSITY_KEY, "yes"));
        }

        COMBINATIONS("Add a command line") {
            commandLine = {"{" + pattern1.getKey() + "}{" + pattern2.getKey() +
                               "}",
                           "blaat/{HELLO}/{" + pattern1.getKey() + "}"};
            REQUIRE(variables.replace(COMMAND_LINE_KEY, commandLine));
        }

        Task expectedTask({PLUGIN_NAME});
        if(verbosity) {
            expectedTask.append("--debug=explain");
        }
        expectedTask.append({"--jobs", to_string(jobs)});
        expectedTask.append(commandLine);

        ExecutorStub::TaskQueue expectedTasks =
            getExpectedTasks(expectedTask, patterns);

        THEN_WHEN("We apply the plugin") {
            Task task;
            bool returnCode = plugin.apply(task, variables, patterns);
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == executor.getExecutedTasks());
            }
        }
    }
}
} // namespace execHelper::plugins::test
