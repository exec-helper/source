#include <map>
#include <string>

#include <gsl/string_span>

#include "config/commandLineOptions.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/commandLine.h"
#include "plugins/executePlugin.h"
#include "plugins/memory.h"
#include "plugins/pluginUtils.h"
#include "plugins/valgrind.h"
#include "plugins/verbosity.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::move;
using std::shared_ptr;
using std::string;

using gsl::czstring;

using execHelper::config::CommandCollection;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::ExecutePlugin;
using execHelper::plugins::Memory;
using execHelper::plugins::MemoryHandler;
using execHelper::plugins::Valgrind;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::utils::getExpectedTasks;

namespace {
const czstring<> PLUGIN_NAME = "valgrind";
const czstring<> MEMORY_KEY = "memory";
const czstring<> TOOL_KEY = "tool";
const czstring<> RUN_COMMAND_KEY = "run-command";
} // namespace

namespace execHelper::plugins::test {
SCENARIO("Obtaining the default variables map of the valgrind plugin",
         "[valgrind]") {
    GIVEN("The default fleeting options") {
        FleetingOptionsStub fleetingOptions;
        Valgrind plugin;

        VariablesMap actualVariables(PLUGIN_NAME);
        REQUIRE(actualVariables.add(COMMAND_LINE_KEY, CommandLineArgs()));
        REQUIRE(actualVariables.add(VERBOSITY_KEY, "no"));

        WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN("We should find the same ones") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO("Test the variables map of the valgrind plugin", "[valgrind]") {
    MAKE_COMBINATIONS("Of several configurations") {
        const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
        const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
        const Patterns patterns({pattern1, pattern2});

        Valgrind plugin;
        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        CommandCollection runCommands({MEMORY_KEY});
        REQUIRE(variables.add(RUN_COMMAND_KEY, MEMORY_KEY));

        CommandLineArgs commandLine;
        string tool;

        MemoryHandler memory;
        SettingsNode settings(PLUGIN_NAME);

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        COMBINATIONS("Add an additional run command") {
            runCommands.emplace_back(MEMORY_KEY);
            REQUIRE(variables.add(RUN_COMMAND_KEY, MEMORY_KEY));
        }

        COMBINATIONS("Set the tool") {
            tool = "tool1";
            REQUIRE(variables.replace(TOOL_KEY, tool));
        }

        COMBINATIONS("Set the command line") {
            commandLine = {"{" + pattern1.getKey() + "}",
                           "{" + pattern2.getKey() + "}"};
            REQUIRE(variables.add(COMMAND_LINE_KEY, commandLine));
        }

        FleetingOptionsStub fleetingOptions;
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(move(settings));
        ExecutePlugin::push(Patterns(patterns));
        ExecutePlugin::push(
            Plugins({{"Memory",
                      shared_ptr<Plugin>(new execHelper::plugins::Memory())}}));

        ExecutorStub::TaskQueue expectedTasks;
        for(const auto& command : runCommands) {
            (void)
                command; // Command is unused, as we only want to consider the part of the command associated with this command
            Task expectedTask({PLUGIN_NAME});
            if(!tool.empty()) {
                expectedTask.append("--tool=" + tool);
            }
            expectedTask.append(commandLine);
            expectedTasks.emplace_back(expectedTask);
        }

        ExecutorStub::TaskQueue replacedTasks =
            getExpectedTasks(expectedTasks, patterns);

        THEN_WHEN("We apply the plugin") {
            Task task;

            bool returnCode = plugin.apply(task, variables, patterns);
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                const Memory::Memories& memories =
                    MemoryHandler::getExecutions();
                REQUIRE(memories.size() == replacedTasks.size());
                auto replacedTask = replacedTasks.begin();
                for(auto memory = memories.begin(); memory != memories.end();
                    ++memory, ++replacedTask) {
                    REQUIRE(memory->task == *replacedTask);
                    REQUIRE(memory->patterns.empty());
                }
            }
        }

        ExecutePlugin::popPlugins();
        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    }
}

SCENARIO("Test erroneous scenarios", "[valgrind]") {
    GIVEN("A configuration without a configured run command") {
        Valgrind plugin;
        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        WHEN("We call the plugin") {
            bool returnCode = plugin.apply(Task(), variables, Patterns());

            THEN("It should fail") { REQUIRE_FALSE(returnCode); }
        }
    }
}
} // namespace execHelper::plugins::test
