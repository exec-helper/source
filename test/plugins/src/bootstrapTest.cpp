#include <string>

#include <gsl/string_span>

#include "config/path.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "log/log.h"
#include "plugins/bootstrap.h"
#include "plugins/buildPlugin.h"
#include "plugins/commandLine.h"
#include "plugins/pluginUtils.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::string;

using gsl::czstring;

using execHelper::config::Path;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::ExecuteCallback;
using execHelper::plugins::registerExecuteCallback;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::utils::getExpectedTasks;

namespace {
const czstring<> PLUGIN_NAME = "bootstrap";
const czstring<> FILENAME_KEY = "filename";
} // namespace

namespace execHelper {
namespace plugins {
namespace test {
SCENARIO("Obtain the plugin name of the bootstrap plugin", "[bootstrap]") {
    log::init();

    GIVEN("A plugin") {
        Bootstrap plugin;

        WHEN("We request the plugin name") {
            const string pluginName = plugin.getPluginName();

            THEN("We should find the correct plugin name") {
                REQUIRE(pluginName == PLUGIN_NAME);
            }
        }
    }
}

SCENARIO("Obtaining the default variables map of the bootstrap plugin",
         "[bootstrap]") {
    GIVEN("The default fleeting options") {
        FleetingOptionsStub fleetingOptions;
        Bootstrap plugin;

        VariablesMap actualVariables(plugin.getPluginName());
        actualVariables.add(Bootstrap::getBuildDirKey(), ".");
        actualVariables.add(COMMAND_LINE_KEY);
        actualVariables.add(FILENAME_KEY, "bootstrap.sh");

        WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN("We should find the same ones") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO("Test the combination of several settings for the bootstrap plugin",
         "[bootstrap]") {
    MAKE_COMBINATIONS("Of several settings") {
        const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
        const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
        const Patterns patterns({pattern1, pattern2});

        Bootstrap plugin;
        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        Task expectedTask;

        auto runCommand = variables.get<Path>(FILENAME_KEY).get();
        auto buildDir = variables.get<Path>(Bootstrap::getBuildDirKey()).get();
        CommandLineArgs commandLine;

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        COMBINATIONS("Set a build directory") {
            buildDir = "{" + pattern1.getKey() + "}/{" + pattern2.getKey() +
                       "}/{HELLO}/{" + pattern2.getKey() + "}/hello{" +
                       pattern1.getKey() + " }world";
            variables.replace(Bootstrap::getBuildDirKey(), buildDir.native());
        }

        COMBINATIONS("Set a different file name") {
            runCommand = "other-file-name.sh";
            variables.replace(FILENAME_KEY, runCommand.native());
        }

        COMBINATIONS("Set a different absolute file name") {
            runCommand = "/other/abosolute/filename.sh";
            variables.replace(FILENAME_KEY, runCommand.native());
        }

        COMBINATIONS("Add a command line") {
            commandLine = {"{" + pattern2.getKey() + "}{" + pattern1.getKey() +
                           "}"};
            variables.replace(COMMAND_LINE_KEY, commandLine);
        }

        expectedTask.setWorkingDirectory(buildDir);
        if(runCommand.is_relative()) {
            runCommand = Path(".") / runCommand;
        }
        expectedTask.append(runCommand.native());
        expectedTask.append(commandLine);

        ExecutorStub::TaskQueue expectedTasks =
            getExpectedTasks(expectedTask, patterns);

        bool returnCode = plugin.apply(Task(), variables, patterns);
        THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

        THEN_CHECK("It called the right commands") {
            REQUIRE(expectedTasks == executor.getExecutedTasks());
        }
    }
}
} // namespace test
} // namespace plugins
} // namespace execHelper
