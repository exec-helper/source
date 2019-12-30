#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "config/commandLineOptions.h"
#include "config/environment.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/executePlugin.h"
#include "plugins/luaPlugin.h"
#include "plugins/memory.h"

#include "base-utils/nonEmptyString.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/commonGenerators.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"
#include "handlers.h"

using std::optional;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::vector;

using execHelper::config::Command;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Patterns;
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
constexpr string_view buildCommandConfigKey{"build-command"};

auto scriptPath() noexcept -> std::string {
    return string(PLUGINS_INSTALL_PATH) + "/clang-static-analyzer.lua";
}
} // namespace

namespace execHelper::plugins::test {
SCENARIO(
    "Testing the configuration settings of the clang-static-analyzer plugin",
    "[clang-static-analyzer]") {
    propertyTest("", [](const optional<filesystem::path>& workingDir,
                        const optional<EnvironmentCollection>& environment,
                        const std::vector<NonEmptyString>& buildCommand,
                        const optional<vector<string>>& commandLine,
                        const optional<bool> verbose) {
        RC_PRE(!buildCommand.empty());
        Patterns patterns;
        const Task task;
        Task expectedTask(task);

        VariablesMap config("clang-static-analyzer-test");
        MemoryHandler memory;

        LuaPlugin plugin(scriptPath());

        Plugins plugins;
        auto memoryPlugin = shared_ptr<Plugin>(new Memory());
        for(const auto& command : buildCommand) {
            REQUIRE(config.add(string(buildCommandConfigKey), *command));
            plugins[*command] = memoryPlugin;
        }

        expectedTask.append("scan-build");

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
        }

        if(verbose) {
            handleVerbosity(*verbose, "-v", config, expectedTask);
        }

        if(commandLine) {
            handleCommandLine(*commandLine, config, expectedTask);
        }

        ExecutorStub::TaskQueue expectedTasks(buildCommand.size(),
                                              expectedTask);

        FleetingOptionsStub fleetingOptions;

        ExecutePlugin::push(std::move(plugins));
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode("clang-static-analyzer-test"));
        ExecutePlugin::push(Patterns(patterns));

        THEN_WHEN("We apply the plugin") {
            Task task;
            bool returnCode = plugin.apply(task, config, patterns);
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                const Memory::Memories& memories =
                    MemoryHandler::getExecutions();
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
        ExecutePlugin::popPlugins();
    });
}

SCENARIO("Testing invalid configurations", "[clang-static-analyzer]") {
    GIVEN("An empty setup") {
        LuaPlugin plugin(scriptPath());
        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        Task task;
        MemoryHandler memory;

        WHEN("We apply the plugin") {
            bool returnCode = plugin.apply(task, variables, Patterns());

            THEN("It should fail") { REQUIRE_FALSE(returnCode); }

            THEN("Nothing should have been executed") {
                const Memory::Memories& memories =
                    MemoryHandler::getExecutions();
                REQUIRE(memories.empty());
            }
        }

        WHEN("We add the build-command as a specific config key without a "
             "value") {
            REQUIRE(variables.add(string(buildCommandConfigKey)));

            bool returnCode = plugin.apply(task, variables, Patterns());
            THEN("It should fail") { REQUIRE_FALSE(returnCode); }
        }
    }
}
} // namespace execHelper::plugins::test
