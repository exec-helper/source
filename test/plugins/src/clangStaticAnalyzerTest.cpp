#include "unittest/catch.h"

#include <string>

#include <gsl/string_span>

#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "log/log.h"
#include "plugins/clangStaticAnalyzer.h"
#include "plugins/commandLine.h"
#include "plugins/executePlugin.h"
#include "plugins/memory.h"
#include "plugins/pluginUtils.h"
#include "plugins/verbosity.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::string;

using gsl::czstring;

using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::plugins::COMMAND_LINE_KEY;
using execHelper::plugins::ExecutePlugin;
using execHelper::plugins::VERBOSITY_KEY;

using execHelper::test::FleetingOptionsStub;

namespace {
const czstring<> PLUGIN_NAME = "clang-static-analyzer";
const czstring<> PLUGIN_COMMAND = "scan-build";
const czstring<> BUILD_COMMAND_KEY = "build-command";
const czstring<> BUILD_DIR_KEY = "build-dir";
const czstring<> MEMORY_KEY = "memory";
} // namespace

namespace execHelper {
namespace plugins {
namespace test {
SCENARIO("Obtain the plugin name of the clang-static-analyzer plugin",
         "[clang-static-analyzer]") {
    log::init();

    GIVEN("A plugin") {
        ClangStaticAnalyzer plugin;

        WHEN("We request the plugin name") {
            const string pluginName = plugin.getPluginName();

            THEN("We should find the correct plugin name") {
                REQUIRE(pluginName == PLUGIN_NAME);
            }
        }
    }
}

SCENARIO("Obtain the default variables map of the clang-static-analyzer plugin",
         "[clang-static-analyzer]") {
    MAKE_COMBINATIONS("of fleeting options") {
        FleetingOptionsStub fleetingOptions;
        ClangStaticAnalyzer plugin;

        VariablesMap actualVariables(plugin.getPluginName());
        actualVariables.add(BUILD_DIR_KEY, ".");
        actualVariables.add(COMMAND_LINE_KEY);
        actualVariables.add(VERBOSITY_KEY, "no");

        COMBINATIONS("Switch on verbosity") {
            fleetingOptions.m_verbose = true;
            actualVariables.replace(VERBOSITY_KEY, "yes");
        }

        THEN_WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN_CHECK("We should find the right one") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO("Successfully applying the clang-static-analyzer plugin",
         "[clang-static-analyzer]") {
    MAKE_COMBINATIONS("Of several settings") {
        const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
        const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
        const Patterns patterns({pattern1, pattern2});

        MemoryHandler memory;

        ClangStaticAnalyzer plugin;
        Task task;

        Task expectedTask({PLUGIN_COMMAND});
        TaskCollection verbosity;
        TaskCollection commandLine;

        FleetingOptionsStub fleetingOptions;
        VariablesMap variables = plugin.getVariablesMap(fleetingOptions);
        variables.add(BUILD_COMMAND_KEY, MEMORY_KEY);

        ExecutePlugin::push(&fleetingOptions);
        ExecutePlugin::push(SettingsNode("test"));
        ExecutePlugin::push(Patterns(patterns));

        COMBINATIONS("Switch on verbosity") {
            variables.replace(VERBOSITY_KEY, "yes");
            verbosity.emplace_back("-v");
        }

        COMBINATIONS("Add a command line") {
            commandLine = {"{" + pattern1.getKey() + "}{" + pattern2.getKey() +
                               "}",
                           "blaat/{HELLO}/{" + pattern1.getKey() + "}"};
            variables.replace(COMMAND_LINE_KEY, commandLine);
        }

        expectedTask.append(verbosity);
        expectedTask.append(commandLine);

        THEN_WHEN("We apply the plugin") {
            bool returnCode = plugin.apply(task, variables, patterns);
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                const Memory::Memories& memories = memory.getExecutions();
                REQUIRE(memories.size() == 1U);
                REQUIRE(memories[0].task == expectedTask);
            }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    }
}

SCENARIO("Testing the invalid configurations", "[clang-static-analyzer]") {
    GIVEN("An empty setup") {
        ClangStaticAnalyzer plugin;
        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        Task task;
        MemoryHandler memory;

        WHEN("We apply the plugin") {
            bool returnCode = plugin.apply(task, variables, Patterns());

            THEN("It should fail") { REQUIRE_FALSE(returnCode); }

            THEN("Nothing should have been executed") {
                const Memory::Memories& memories = memory.getExecutions();
                REQUIRE(memories.empty());
            }
        }

        WHEN("We add the build-command as a specific config key without a "
             "value") {
            variables.add(BUILD_COMMAND_KEY);

            bool returnCode = plugin.apply(task, variables, Patterns());
            THEN("It should fail") { REQUIRE_FALSE(returnCode); }
        }
    }
}
} // namespace test
} // namespace plugins
} // namespace execHelper
