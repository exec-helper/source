#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "config/commandLineOptions.h"
#include "config/environment.h"
#include "config/pattern.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/executionContext.h"
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
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;

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
    const FleetingOptionsStub options;
    const SettingsNode settings("clang-static-analyzer-test");
    const PatternsHandler patterns;

    propertyTest("", [&options, &settings, &patterns](
                         const optional<filesystem::path>& workingDir,
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

        ExecutionContext context(options, settings, patterns, plugins);

        expectedTask.append("scan-build");

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
        }

        handleVerbosity(verbose ? *verbose : context.options().getVerbosity(),
                        "-v", config, expectedTask);

        if(commandLine) {
            handleCommandLine(*commandLine, config, expectedTask);
        }

        Tasks expectedTasks(buildCommand.size(), expectedTask);

        THEN_WHEN("We apply the plugin") {
            auto actualTasks = plugin.apply(task, config, context);

            THEN_CHECK("It called the right commands") {
                REQUIRE(actualTasks == expectedTasks);
            }
        }
    });
}

SCENARIO("Testing invalid configurations", "[clang-static-analyzer][error]") {
    GIVEN("An empty setup") {
        LuaPlugin plugin(scriptPath());

        FleetingOptionsStub options;
        SettingsNode settings("clang-static-analyzer");
        PatternsHandler patterns;
        Plugins plugins;
        ExecutionContext context(options, settings, patterns, plugins);

        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        Task task;

        WHEN("We apply the plugin") {
            THEN("It should throw an exception") {
                REQUIRE_THROWS_AS(plugin.apply(task, variables, context),
                                  runtime_error);
            }
        }

        WHEN("We add the build-command as a specific config key without a "
             "value") {
            REQUIRE(variables.add(string(buildCommandConfigKey)));

            THEN("It should fail") {
                REQUIRE_THROWS_AS(plugin.apply(task, variables, context),
                                  runtime_error);
            }
        }
    }
}
} // namespace execHelper::plugins::test
