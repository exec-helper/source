#include <filesystem>
#include <string>

#include <gsl/string_span>

#include "config/environment.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "plugins/commandLine.h"
#include "plugins/make.h"
#include "plugins/pluginUtils.h"
#include "plugins/threadedness.h"
#include "plugins/verbosity.h"
#include "plugins/workingDirectory.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::string;
using std::to_string;

using gsl::czstring;

using execHelper::config::EnvArgs;
using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Path;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::utils::getExpectedTasks;

namespace filesystem = std::filesystem;

namespace {
const czstring<> PLUGIN_NAME = "make";
const czstring<> BUILD_DIR_KEY = "build-dir";
} // namespace

namespace execHelper {
namespace plugins {
namespace test {
SCENARIO("Obtain the plugin name of the make plugin", "[make]") {
    GIVEN("A plugin") {
        Make plugin;

        WHEN("We request the plugin name") {
            const string pluginName = plugin.getPluginName();

            THEN("We should find the correct plugin name") {
                REQUIRE(pluginName == PLUGIN_NAME);
            }
        }
    }
}

SCENARIO("Obtaining the default variables map of the make plugin", "[make]") {
    MAKE_COMBINATIONS("The default fleeting options") {
        FleetingOptionsStub fleetingOptions;
        Make plugin;

        VariablesMap actualVariables(plugin.getPluginName());
        actualVariables.add(COMMAND_LINE_KEY, CommandLineArgs());
        actualVariables.add(VERBOSITY_KEY, "no");
        actualVariables.add(JOBS_KEY, to_string(fleetingOptions.m_jobs));
        actualVariables.add(BUILD_DIR_KEY, ".");
        actualVariables.add(ENVIRONMENT_KEY, EnvArgs());

        COMBINATIONS("Switch off multithreading") {
            fleetingOptions.m_jobs = 1U;
            actualVariables.replace(JOBS_KEY,
                                    to_string(fleetingOptions.m_jobs));
        }

        COMBINATIONS("Switch on verbosity") {
            fleetingOptions.m_verbose = true;
            actualVariables.replace(VERBOSITY_KEY, "yes");
        }

        THEN_WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN_CHECK("We should find the same ones") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO("Testing the configuration settings of the make plugin", "[make]") {
    MAKE_COMBINATIONS("Of several settings") {
        const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
        const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
        const Patterns patterns({pattern1, pattern2});

        Make plugin;
        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        auto jobs = variables.get<Jobs>(JOBS_KEY).value();
        Path buildDir = variables.get<Path>(BUILD_DIR_KEY).value();
        bool verbosity = false;
        CommandLineArgs commandLine;
        EnvironmentCollection env;
        Path workingDirectory(filesystem::current_path());

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        COMBINATIONS("Switch off multi-threading") {
            jobs = 1U;
            variables.replace(JOBS_KEY, to_string(jobs));
        }

        COMBINATIONS("Set a build directory") {
            buildDir = "{" + pattern1.getKey() + "}/{" + pattern2.getKey() +
                       "}/{HELLO}/{" + pattern2.getKey() + "}/hello{" +
                       pattern1.getKey() + " }world";
            variables.replace(BUILD_DIR_KEY, buildDir.native());
        }

        COMBINATIONS("Switch on verbosity") {
            verbosity = true;
            variables.replace(VERBOSITY_KEY, "yes");
        }

        COMBINATIONS("Add a command line") {
            commandLine = {"{" + pattern2.getKey() + "}",
                           "{" + pattern1.getKey() + "}"};
            variables.replace(COMMAND_LINE_KEY, commandLine);
        }

        COMBINATIONS("Set environment") {
            env = {{"VAR1", "value1"}, {"VAR2", "value2"}};
            for(const auto& value : env) {
                variables.replace({ENVIRONMENT_KEY, value.first}, value.second);
            }
        }

        COMBINATIONS("Set the working directory") {
            workingDirectory = "1234{" + pattern2.getKey() + "}/4321/{HELLO}";
            variables.replace(WORKING_DIR_KEY, workingDirectory.native());
        }

        COMBINATIONS("Set the working directory to the current directory") {
            workingDirectory = ".";
            variables.replace(WORKING_DIR_KEY, workingDirectory.native());
        }

        Task expectedTask({PLUGIN_NAME});
        expectedTask.append({"--directory", buildDir.native()});
        expectedTask.append({"--jobs", to_string(jobs)});
        if(verbosity) {
            expectedTask.append("--debug");
        }
        expectedTask.append(commandLine);
        expectedTask.setEnvironment(env);
        expectedTask.setWorkingDirectory(workingDirectory);

        ExecutorStub::TaskQueue expectedTasks =
            getExpectedTasks(expectedTask, patterns);
        THEN_WHEN("We apply the plugin") {
            bool returnCode = plugin.apply(Task(), variables, patterns);
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == executor.getExecutedTasks());
            }
        }
    }
}
} // namespace test
} // namespace plugins
} // namespace execHelper
