#include <string>

#include <gsl/string_span>

#include "config/commandLineOptions.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/commandPlugin.h"
#include "plugins/executePlugin.h"
#include "plugins/memory.h"
#include "plugins/pluginUtils.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "fleetingOptionsStub.h"

using std::string;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::CommandCollection;
using execHelper::config::Patterns;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;

namespace {
const czstring<> PLUGIN_NAME = "commands";
const czstring<> MEMORY_KEY = "memory";
} // namespace

namespace execHelper {
namespace plugins {
namespace test {
SCENARIO("Obtain the plugin name of the command-plugin", "[command-plugin]") {
    GIVEN("A plugin") {
        CommandPlugin plugin;

        WHEN("We request the plugin name") {
            const string pluginName = plugin.getPluginName();

            THEN("We should find the correct plugin name") {
                REQUIRE(pluginName == PLUGIN_NAME);
            }
        }
    }
}

SCENARIO("Obtaining the default variables map of the command-plugin",
         "[command-plugin]") {
    MAKE_COMBINATIONS("The default fleeting options") {
        FleetingOptionsStub fleetingOptions;
        CommandPlugin plugin;

        VariablesMap actualVariables(plugin.getPluginName());
        actualVariables.add(PLUGIN_NAME, CommandCollection());

        COMBINATIONS("Add a command") {
            const Command command = "command1";
            fleetingOptions.m_commands = {command};
            actualVariables.replace(PLUGIN_NAME, command);
        }

        COMBINATIONS("Add multiple commands") {
            const CommandCollection commands = {"command1a", "command1b"};
            fleetingOptions.m_commands = commands;
            actualVariables.replace(PLUGIN_NAME, commands);
        }

        THEN_WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN_CHECK("We should find the same ones") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO("Test the commandPlugin plugin", "[command-plugin]") {
    MAKE_COMBINATIONS("Of several options and configurations") {
        MemoryHandler memory;

        Task task;
        CommandPlugin plugin;

        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());
        ExecutorStub::TaskQueue expectedTasks;

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        FleetingOptionsStub fleetingOptions;
        ExecutePlugin::push(gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode(PLUGIN_NAME));
        ExecutePlugin::push(Patterns());

        COMBINATIONS("Add a command to execute") {
            variables.add(PLUGIN_NAME, MEMORY_KEY);
            expectedTasks.emplace_back(Task());
        }

        COMBINATIONS("Add multiple commands to execute") {
            const unsigned int NB_OF_COMMANDS = 5U;
            for(unsigned int i = 0U; i < NB_OF_COMMANDS; ++i) {
                variables.add(PLUGIN_NAME, MEMORY_KEY);
                expectedTasks.emplace_back(Task());
            }
        }

        THEN_WHEN("We apply the plugin") {
            bool return_code = plugin.apply(task, variables, Patterns());

            THEN_CHECK("The call should succeed") { REQUIRE(return_code); }

            THEN_CHECK("All expected actions should be executed") {
                const Memory::Memories& memories = memory.getExecutions();
                REQUIRE(memories.size() == expectedTasks.size());
                auto taskIterator = expectedTasks.begin();
                for(auto memoryIterator = memories.begin();
                    memoryIterator != memories.end();
                    ++memoryIterator, ++taskIterator) {
                    REQUIRE(memoryIterator->task == task);
                }
            }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    }
}
} // namespace test
} // namespace plugins
} // namespace execHelper
