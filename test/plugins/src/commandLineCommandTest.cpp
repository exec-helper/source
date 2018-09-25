#include <algorithm>
#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/environment.h"
#include "config/path.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/commandLine.h"
#include "plugins/commandLineCommand.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::EnvArgs;
using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::EnvironmentValue;
using execHelper::config::Path;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::SettingsKey;
using execHelper::config::SettingsKeys;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::plugins::COMMAND_LINE_KEY;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::utils::getExpectedTasks;

namespace {
const czstring<> PLUGIN_NAME("command-line-command");
const czstring<> WORKING_DIR_KEY("working-dir");
} // namespace

namespace execHelper {
namespace plugins {
namespace test {
SCENARIO("Obtain the plugin name of the command-line-command plugin",
         "[command-line-command]") {
    GIVEN("A plugin") {
        CommandLineCommand plugin;

        WHEN("We request the plugin name") {
            const string pluginName = plugin.getPluginName();

            THEN("We should find the correct plugin name") {
                REQUIRE(pluginName == PLUGIN_NAME);
            }
        }
    }
}

SCENARIO(
    "Obtaining the default variables map of the command-line-command plugin",
    "[command-line-command]") {
    GIVEN("The default fleeting options") {
        FleetingOptionsStub fleetingOptions;
        CommandLineCommand plugin;

        VariablesMap actualVariables(plugin.getPluginName());
        actualVariables.add(COMMAND_LINE_KEY, CommandLineArgs());
        actualVariables.add(ENVIRONMENT_KEY, EnvArgs());

        WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN("We should find the same ones") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO(
    "Testing the configuration settings of the command-line-command plugin",
    "[command-line-command]") {
    MAKE_COMBINATIONS("Of several settings") {
        const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
        const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
        const Pattern pattern3("PATTERN3", {"value3a"});
        const Patterns patterns({pattern1, pattern2, pattern3});

        const string commandKey("command1");
        const CommandLineArgs command1({"command1"});

        CommandLineCommand plugin;
        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());
        Task expectedTask;
        std::vector<TaskCollection> commandLines;

        variables.add(COMMAND_LINE_KEY, command1);
        commandLines.push_back(command1);

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        COMBINATIONS("Set multiple command lines") {
            const CommandLineArgs multipleCommand1({"multiple-commandA"});
            const CommandLineArgs multipleCommand2(
                {"{" + pattern1.getKey() + "}/{" + pattern2.getKey() + "}",
                 "{" + pattern3.getKey() + "}/{" + pattern1.getKey() + "}"});

            variables.clear(COMMAND_LINE_KEY);
            variables.add(SettingsKeys({COMMAND_LINE_KEY, "multiple-commandA"}),
                          multipleCommand1);
            variables.add({COMMAND_LINE_KEY, "multiple-commandB"},
                          multipleCommand2);

            commandLines.clear();
            commandLines.push_back(multipleCommand1);
            commandLines.push_back(multipleCommand2);
        }

        COMBINATIONS("Set environment") {
            EnvironmentValue ENV1("VAR1", "environmentValue{" +
                                              pattern1.getKey() + "}");
            EnvironmentValue ENV2("VAR2", "environmentValue2");
            variables.add({ENVIRONMENT_KEY, ENV1.first}, ENV1.second);
            variables.add({ENVIRONMENT_KEY, ENV2.first}, ENV2.second);
            expectedTask.appendToEnvironment(move(ENV1));
            expectedTask.appendToEnvironment(move(ENV2));
        }

        COMBINATIONS("Set the working directory") {
            variables.replace(WORKING_DIR_KEY, "{" + pattern2.getKey() + "}/{" +
                                                   pattern3.getKey() + "}");
            expectedTask.setWorkingDirectory(
                variables.get<Path>(WORKING_DIR_KEY).value());
        }

        ExecutorStub::TaskQueue unreplacedTasks;
        for(const auto& commandLine : commandLines) {
            Task newTask = expectedTask;
            newTask.append(commandLine);
            unreplacedTasks.emplace_back(newTask);
        }
        const ExecutorStub::TaskQueue expectedTasks =
            getExpectedTasks(unreplacedTasks, patterns);

        Task task;
        bool returnCode = plugin.apply(task, variables, patterns);
        THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

        THEN_CHECK("It called the right commands") {
            REQUIRE(expectedTasks == executor.getExecutedTasks());
        }
    }
}

SCENARIO("Testing erroneous configuration conditions for the "
         "commandLineCommand plugin",
         "[command-line-command]") {
    MAKE_COMBINATIONS("Of erroneous setups") {
        Task task;
        CommandLineCommand plugin;

        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        COMBINATIONS("Add command line key without value") {
            variables.add(COMMAND_LINE_KEY);
        }

        THEN_WHEN("We add no parameter and apply") {
            bool return_code = plugin.apply(task, variables, Patterns());

            THEN_CHECK("The call should not succeed") {
                REQUIRE_FALSE(return_code);
            }
        }
    }
}
} // namespace test
} // namespace plugins
} // namespace execHelper
