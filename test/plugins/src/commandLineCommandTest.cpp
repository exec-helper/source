#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl>

#include "config/environment.h"
#include "config/path.h"
#include "config/pattern.h"
#include "config/patternsHandler.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/commandLine.h"
#include "plugins/commandLineCommand.h"
#include "plugins/executionContext.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::runtime_error;
using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::EnvArgs;
using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::EnvironmentValue;
using execHelper::config::Path;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsKey;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::Tasks;
using execHelper::plugins::COMMAND_LINE_KEY;

using execHelper::test::FleetingOptionsStub;

namespace {
const czstring PLUGIN_NAME("command-line-command");
const czstring WORKING_DIR_KEY("working-dir");
} // namespace

namespace execHelper::plugins::test {
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

        FleetingOptionsStub options;
        VariablesMap variables(PLUGIN_NAME);
        Task expectedTask;
        std::vector<TaskCollection> commandLines;

        REQUIRE(variables.add(string(COMMAND_LINE_KEY), command1));
        commandLines.push_back(command1);

        COMBINATIONS("Set multiple command lines") {
            const CommandLineArgs multipleCommand1({"multiple-commandA"});
            const CommandLineArgs multipleCommand2(
                {"{" + pattern1.getKey() + "}/{" + pattern2.getKey() + "}",
                 "{" + pattern3.getKey() + "}/{" + pattern1.getKey() + "}"});

            variables.clear(COMMAND_LINE_KEY);
            REQUIRE(variables.add(
                SettingsKeys({string(COMMAND_LINE_KEY), "multiple-commandA"}),
                multipleCommand1));
            REQUIRE(
                variables.add({string(COMMAND_LINE_KEY), "multiple-commandB"},
                              multipleCommand2));

            commandLines.clear();
            commandLines.push_back(multipleCommand1);
            commandLines.push_back(multipleCommand2);
        }

        COMBINATIONS("Set environment") {
            EnvironmentValue ENV1("VAR1", "environmentValue{" +
                                              pattern1.getKey() + "}");
            EnvironmentValue ENV2("VAR2", "environmentValue2");
            REQUIRE(variables.add({string(ENVIRONMENT_KEY), ENV1.first},
                                  ENV1.second));
            REQUIRE(variables.add({string(ENVIRONMENT_KEY), ENV2.first},
                                  ENV2.second));
            expectedTask.appendToEnvironment(move(ENV1));
            expectedTask.appendToEnvironment(move(ENV2));
        }

        COMBINATIONS("Set the working directory") {
            REQUIRE(variables.replace(WORKING_DIR_KEY,
                                      "{" + pattern2.getKey() + "}/{" +
                                          pattern3.getKey() + "}"));
            expectedTask.setWorkingDirectory(
                variables.get<Path>(string(WORKING_DIR_KEY)).value());
        }

        Tasks expectedTasks;
        expectedTasks.reserve(commandLines.size());
        for(const auto& commandLine : commandLines) {
            Task newTask = expectedTask;
            newTask.append(commandLine);
            newTask.addPatterns(patterns);
            expectedTasks.emplace_back(newTask);
        }

        SettingsNode settings("command-line-command");
        Plugins plugins;
        PatternsHandler patternsHandler;
        ExecutionContext context(options, settings, patternsHandler, plugins);

        Task task;
        task.addPatterns(patterns);
        auto actualTasks = commandLineCommand(task, variables, context);

        THEN_CHECK("It called the right commands") {
            REQUIRE(expectedTasks == actualTasks);
        }
    }
}

SCENARIO("Testing erroneous configuration conditions for the "
         "commandLineCommand plugin",
         "[command-line-command]") {
    MAKE_COMBINATIONS("Of erroneous setups") {
        Task task;

        FleetingOptionsStub options;
        SettingsNode settings("command-line-command");
        Plugins plugins;
        PatternsHandler patternsHandler;
        ExecutionContext context(options, settings, patternsHandler, plugins);

        VariablesMap variables(PLUGIN_NAME);

        COMBINATIONS("Add command line key without value") {
            REQUIRE(variables.add(string(COMMAND_LINE_KEY)));
        }

        THEN_WHEN("We add no parameter and apply") {
            THEN_CHECK("The plugin should throw an exception") {
                REQUIRE_THROWS_AS(commandLineCommand(task, variables, context),
                                  runtime_error);
            }
        }
    }
}
} // namespace execHelper::plugins::test
