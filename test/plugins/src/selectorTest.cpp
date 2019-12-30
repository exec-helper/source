#include <algorithm>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "config/commandLineOptions.h"
#include "config/environment.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/executePlugin.h"
#include "plugins/logger.h"
#include "plugins/luaPlugin.h"
#include "plugins/memory.h"

#include "base-utils/nonEmptyString.h"
#include "config/generators.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/commonGenerators.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"
#include "handlers.h"

using std::back_inserter;
using std::fill_n;
using std::optional;
using std::shared_ptr;
using std::string;
using std::string_view;

using execHelper::config::Command;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Pattern;
using execHelper::config::PatternKey;
using execHelper::config::Patterns;
using execHelper::config::PatternValues;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::ExecutePlugin;
using execHelper::plugins::MemoryHandler;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::NonEmptyString;
using execHelper::test::propertyTest;

namespace filesystem = std::filesystem;

namespace {
auto scriptPath() noexcept -> std::string {
    return string(PLUGINS_INSTALL_PATH) + "/selector.lua";
}
} // namespace

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the selector plugin",
         "[selector]") {
    propertyTest("", [](const optional<filesystem::path>& workingDir,
                        const optional<EnvironmentCollection>& environment,
                        const NonEmptyString& patternKey,
                        const uint8_t nbOfRepeats, Patterns patterns) {
        const string patternConfigKey{"patterns"};
        const string memoryKey{"memory"};
        const Task task;
        Task expectedTask(task);

        PatternValues commands;
        commands.reserve(nbOfRepeats);
        fill_n(back_inserter(commands), nbOfRepeats, memoryKey);
        Pattern pattern{*patternKey, commands};
        patterns.emplace_back(pattern);

        VariablesMap config("selector-test");
        MemoryHandler memory;

        LuaPlugin plugin(scriptPath());
        REQUIRE(config.add(patternConfigKey, *patternKey));

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
        }

        ExecutorStub::TaskQueue expectedTasks;
        expectedTasks.reserve(nbOfRepeats);
        fill_n(back_inserter(expectedTasks), nbOfRepeats, expectedTask);

        FleetingOptionsStub fleetingOptions;

        ExecutePlugin::push(
            Plugins{{string(memoryKey), shared_ptr<Plugin>(new Memory())}});
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode("selector-test"));
        ExecutePlugin::push(Patterns(patterns));

        THEN_WHEN("We apply the plugin") {
            Task task;
            bool returnCode = plugin.apply(task, config, patterns);
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                const Memory::Memories& memories =
                    MemoryHandler::getExecutions();
                REQUIRE(memories.size() == nbOfRepeats);
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
        ExecutePlugin::popPlugins();
    });
}

SCENARIO("Undefined pattern configuration", "[selector]") {
    GIVEN("A config without a pattern defined") {
        LuaPlugin plugin(scriptPath());

        FleetingOptionsStub fleetingOptions;
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode("selector-test"));
        ExecutePlugin::push(Patterns());

        WHEN("We call the plugin") {
            bool returnCode =
                plugin.apply(Task(), VariablesMap("selector-test"), Patterns());

            THEN("It should fail") { REQUIRE_FALSE(returnCode); }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    }
}

SCENARIO("Select an unknown pattern", "[selector]") {
    propertyTest("", [](const Patterns& patterns,
                        const PatternKey& patternKey) {
        const string patternConfigKey{"patterns"};

        LuaPlugin plugin(scriptPath());

        VariablesMap config("selector-test");
        REQUIRE(config.add(patternConfigKey, patternKey));

        FleetingOptionsStub fleetingOptions;
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode("selector-test"));
        ExecutePlugin::push(Patterns(patterns));

        WHEN("We call the plugin") {
            bool returnCode =
                plugin.apply(Task(), VariablesMap("selector-test"), patterns);

            THEN("It should fail") { REQUIRE_FALSE(returnCode); }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    });
}
} // namespace execHelper::plugins::test
