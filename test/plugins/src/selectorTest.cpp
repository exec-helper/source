#include <string>

#include <gsl/string_span>

#include "config/commandLineOptions.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "log/log.h"
#include "plugins/executePlugin.h"
#include "plugins/logger.h"
#include "plugins/memory.h"
#include "plugins/selector.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::string;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::CommandCollection;
using execHelper::config::Pattern;
using execHelper::config::PatternKey;
using execHelper::config::PatternValues;
using execHelper::config::Patterns;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::ExecutePlugin;
using execHelper::plugins::MemoryHandler;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;

namespace {
const czstring<> PLUGIN_NAME = "selector";
const czstring<> PATTERN_KEY = "patterns";
const czstring<> MEMORY_KEY = "memory";
} // namespace

namespace execHelper {
namespace plugins {
namespace test {
SCENARIO("Obtain the plugin name of the selector plugin", "[selector]") {
    log::init();

    GIVEN("A plugin") {
        Selector plugin;

        WHEN("We request the plugin name") {
            const string pluginName = plugin.getPluginName();

            THEN("We should find the correct plugin name") {
                REQUIRE(pluginName == PLUGIN_NAME);
            }
        }
    }
}

SCENARIO("Obtaining the default variables map of the selector plugin",
         "[selector]") {
    MAKE_COMBINATIONS("of fleeting options") {
        FleetingOptionsStub fleetingOptions;
        Selector plugin;

        VariablesMap actualVariables(plugin.getPluginName());

        THEN_WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN_CHECK("We should find the right one") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO("Make combinations of configurations for the selector plugin") {
    MAKE_COMBINATIONS("Of several configurations") {
        const Pattern pattern1("PATTERN1", {MEMORY_KEY});
        const Pattern pattern2("PATTERN2", {MEMORY_KEY, MEMORY_KEY});
        Patterns patterns({pattern1});

        Command command = "selector-command";

        MemoryHandler memory;

        Selector plugin;

        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());
        variables.add(PATTERN_KEY, pattern1.getKey());

        ExecutorStub::TaskQueue expectedTasks;
        expectedTasks.emplace_back(Task());

        COMBINATIONS("Add another select entry") {
            PatternValues newValues = patterns.front().getValues();
            newValues.emplace_back(MEMORY_KEY);
            patterns.front().setValues(newValues);
            expectedTasks.emplace_back(Task());
        }

        COMBINATIONS("Add another pattern entry") {
            variables.add(PATTERN_KEY, pattern2.getKey());
            patterns.push_back(pattern2);
            for(const auto& entry : pattern2.getValues()) {
                expectedTasks.emplace_back(Task());
            }
        }

        FleetingOptionsStub fleetingOptions;
        ExecutePlugin::push(&fleetingOptions);
        ExecutePlugin::push(SettingsNode(PLUGIN_NAME));
        ExecutePlugin::push(Patterns(patterns));

        THEN_WHEN("We apply the plugin") {
            Task task;
            bool returnCode = plugin.apply(task, variables, patterns);
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                const Memory::Memories& memories = memory.getExecutions();
                REQUIRE(memories.size() == expectedTasks.size());
                auto expectedTask = expectedTasks.begin();
                for(auto memory = memories.begin(); memory != memories.end();
                    ++memory, ++expectedTask) {
                    REQUIRE(memory->task == *expectedTask);
                    REQUIRE(memory->patterns.empty());
                }
            }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    }
}

SCENARIO("Erroneous situations", "[selector]") {
    GIVEN("A config without select commands defined") {
        Selector plugin;
        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        WHEN("We call the plugin") {
            bool returnCode = plugin.apply(Task(), variables, Patterns());

            THEN("It should fail") { REQUIRE_FALSE(returnCode); }
        }
    }
}
} // namespace test
} // namespace plugins
} // namespace execHelper
