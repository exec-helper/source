#include <experimental/array>
#include <optional>
#include <string>
#include <vector>

#include "config/environment.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/executionContext.h"
#include "plugins/luaPlugin.h"

#include "core/coreGenerators.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/addToConfig.h"
#include "utils/addToTask.h"
#include "utils/commonGenerators.h"

#include "fleetingOptionsStub.h"
#include "handlers.h"

using namespace std;

using execHelper::config::EnvironmentCollection;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::Tasks;

using execHelper::test::addToConfig;
using execHelper::test::addToTask;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::propertyTest;

namespace {
enum class Shell { Sh, Bash, Zsh, Fish };

constexpr auto getAllShells() {
    return std::experimental::make_array(Shell::Sh, Shell::Bash, Shell::Zsh,
                                         Shell::Fish);
}

inline auto shellToString(Shell shell) {
    switch(shell) {
    case Shell::Sh:
        return "sh"sv;
    case Shell::Bash:
        return "bash"sv;
    case Shell::Zsh:
        return "zsh"sv;
    case Shell::Fish:
        return "fish"sv;
    default:
        throw runtime_error("Invalid shell");
    }
}
} // namespace

namespace rc {
template <> struct Arbitrary<Shell> {
    static auto arbitrary() { return gen::elementOf(getAllShells()); };
};
} // namespace rc

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the shell plugins",
         "[shell][successful]") {
    const FleetingOptionsStub options;
    const SettingsNode settings("settings-shell");
    const PatternsHandler patterns;
    const Plugins plugins;
    const ExecutionContext context(options, settings, patterns, plugins);

    propertyTest("", [&context](
                         Shell shell, const Task& task,
                         const optional<filesystem::path>& workingDir,
                         const optional<vector<string>>& commandLine,
                         const optional<EnvironmentCollection>& environment,
                         const string& command) {
        auto expectedTask = task;

        auto shellName = string(shellToString(shell));
        expectedTask.append(shellName);

        VariablesMap config("shell-test");

        addToConfig("command", command, config);
        addToTask(command, expectedTask, [](const auto& cmd) -> TaskCollection {
            return {"-c", cmd};
        });

        if(commandLine) {
            handleCommandLine(*commandLine, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
        }

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        THEN_WHEN("We apply the plugin") {
            auto actualTasks = luaPlugin(task, config, context,
                                         std::string(PLUGINS_INSTALL_PATH) +
                                             "/" + shellName + ".lua");

            THEN_CHECK("It generated the expected tasks") {
                REQUIRE(actualTasks == Tasks({expectedTask}));
            }
        }
    });
}

SCENARIO("Pass no command to the shell plugin", "[shell][error]") {
    const FleetingOptionsStub options;
    const SettingsNode settings("settings-shell");
    const PatternsHandler patternsHandler;
    const Plugins plugins;
    const ExecutionContext context(options, settings, patternsHandler, plugins);

    propertyTest("", [&context](Shell shell, const Task& task) {
        auto shellName = string(shellToString(shell));

        VariablesMap config("shell-test");

        THEN_WHEN("We call the shell plugin with this configuration") {
            THEN_CHECK("It throws a runtime error") {
                REQUIRE_THROWS_AS(luaPlugin(task, config, context,
                                            std::string(PLUGINS_INSTALL_PATH) +
                                                "/" + shellName + ".lua"),
                                  runtime_error);
            }
        }
    });
}
} // namespace execHelper::plugins::test
