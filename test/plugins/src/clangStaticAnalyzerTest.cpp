#include <memory>
#include <optional>
#include <stdexcept>
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

#include "fleetingOptionsStub.h"
#include "handlers.h"

using std::optional;
using std::runtime_error;
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
using execHelper::core::Tasks;
using execHelper::plugins::ExecutePlugin;

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
    "[clang-static-analyzer][success]") {
    propertyTest("", [](const optional<filesystem::path>& workingDir,
                        const optional<EnvironmentCollection>& environment,
                        const std::vector<NonEmptyString>& buildCommand,
                        const optional<vector<string>>& commandLine,
                        const optional<bool> verbose) {
        RC_PRE(!buildCommand.empty());
        const Task task;

        Task expectedTask = task;

        VariablesMap config("clang-static-analyzer-test");

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

        Tasks expectedTasks(buildCommand.size(), expectedTask);

        FleetingOptionsStub fleetingOptions;

        auto plRaii = ExecutePlugin::push(std::move(plugins));
        auto flRaii = ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        auto seRaii =
            ExecutePlugin::push(SettingsNode("clang-static-analyzer-test"));
        auto paRaii = ExecutePlugin::push(Patterns(task.getPatterns()));

        THEN_WHEN("We apply the plugin") {
            auto actualTasks = plugin.apply(task, config);

            THEN_CHECK("It called the right commands") {
                REQUIRE(actualTasks == expectedTasks);
            }
        }
    });
}

SCENARIO("Testing invalid configurations", "[clang-static-analyzer][error]") {
    GIVEN("An empty setup") {
        LuaPlugin plugin(scriptPath());
        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        Task task;

        WHEN("We apply the plugin") {
            THEN("It should throw an exception") {
                REQUIRE_THROWS_AS(plugin.apply(task, variables), runtime_error);
            }
        }

        WHEN("We add the build-command as a specific config key without a "
             "value") {
            REQUIRE(variables.add(string(buildCommandConfigKey)));

            THEN("It should fail") {
                REQUIRE_THROWS_AS(plugin.apply(task, variables), runtime_error);
            }
        }
    }
}
} // namespace execHelper::plugins::test
