#include <array>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <gsl/pointers>

#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "plugins/executePlugin.h"
#include "plugins/luaPlugin.h"

#include "config/generators.h"
#include "core/coreGenerators.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/addToConfig.h"
#include "utils/addToTask.h"
#include "utils/commonGenerators.h"
#include "utils/utils.h"

#include "fleetingOptionsStub.h"
#include "handlers.h"

namespace filesystem = std::filesystem;

using std::array;
using std::optional;
using std::runtime_error;
using std::string;
using std::string_view;
using std::vector;

using gsl::not_null;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::Tasks;

using execHelper::test::addToConfig;
using execHelper::test::addToTask;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::propertyTest;
using execHelper::test::utils::registerValuesAsCommands;

namespace {
enum class Tool {
    Memcheck,
    Cachegrind,
    Callgrind,
    Helgrind,
    Drd,
    Massif,
    Dhat,
    Lackey,
    None,
    Exp_bbv
};

constexpr size_t nbOfTools = 10U;

constexpr auto getAllTools() {
    return array<Tool, nbOfTools>({Tool::Memcheck, Tool::Cachegrind,
                                   Tool::Callgrind, Tool::Helgrind, Tool::Drd,
                                   Tool::Massif, Tool::Dhat, Tool::Lackey,
                                   Tool::None, Tool::Exp_bbv});
}

constexpr auto toString(Tool tool) -> string_view {
    using namespace std::literals;

    switch(tool) {
    case Tool::Memcheck:
        return "memcheck"sv;
    case Tool::Cachegrind:
        return "cachegrind"sv;
    case Tool::Callgrind:
        return "callgrind"sv;
    case Tool::Helgrind:
        return "helgrind"sv;
    case Tool::Drd:
        return "drd"sv;
    case Tool::Massif:
        return "Massif"sv;
    case Tool::Dhat:
        return "dhat"sv;
    case Tool::Lackey:
        return "lackey"sv;
    case Tool::None:
        return "none"sv;
    case Tool::Exp_bbv:
        return "exp-bbv"sv;
    }
    throw runtime_error(
        "Failed to convert the given tool to a string representation");
}

inline void addToConfig(const execHelper::config::SettingsKeys& key,
                        const Tool tool, not_null<VariablesMap*> config) {
    execHelper::test::addToConfig(key, toString(tool), config);
}

inline void addToTask(const Tool tool, not_null<execHelper::core::Task*> task,
                      execHelper::test::AddToTaskFunction func) {
    execHelper::test::addToTask(toString(tool), task, move(func));
}
} // namespace

namespace rc {
template <> struct Arbitrary<Tool> {
    static auto arbitrary() -> Gen<Tool> {
        return gen::elementOf(getAllTools());
    };
};
} // namespace rc

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the valgrind plugin",
         "[valgrind]") {
    const FleetingOptionsStub options;
    const SettingsNode settings("valgrind");

    propertyTest("", [&options, &settings](
                         const optional<Tool>& tool,
                         const optional<filesystem::path>& workingDir,
                         const optional<vector<string>>& commandLine,
                         const optional<EnvironmentCollection>& environment,
                         const optional<bool> verbose, const Pattern& pattern,
                         Task task) {
        task.addPatterns({pattern});

        Task expectedTask = task;
        expectedTask.append("valgrind");

        VariablesMap config("valgrind-test");

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/valgrind.lua");

        Plugins plugins;
        auto memories = registerValuesAsCommands(pattern.getValues(), &plugins);

        PatternsHandler patternsHandler;
        patternsHandler.addPattern(pattern);
        const ExecutionContext context(options, settings, patternsHandler,
                                       plugins);

        auto runCommand = string("{").append(pattern.getKey()).append("}");
        addToConfig("run-command", runCommand, &config);

        addToConfig("tool", tool, &config);
        addToTask(tool, &expectedTask, [](const auto& tool) -> TaskCollection {
            return {std::string("--tool=").append(tool)};
        });

        handleVerbosity(verbose ? *verbose : context.options().getVerbosity(),
                        "--verbose", config, expectedTask);

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
        }

        if(commandLine) {
            handleCommandLine(*commandLine, config, expectedTask);
        }

        Tasks expectedTasks(pattern.getValues().size(), expectedTask);

        THEN_WHEN("We apply the plugin") {
            auto actualTasks = plugin.apply(task, config, context);

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == actualTasks);
            }
        }
    });
}

SCENARIO("Test erroneous scenarios for the valgrind plugin", "[valgrind]") {
    GIVEN("A configuration without a configured run command") {
        FleetingOptionsStub options;
        SettingsNode settings("settings");
        Plugins plugins;
        PatternsHandler patternsHandler;
        const ExecutionContext context(options, settings, patternsHandler,
                                       plugins);

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/valgrind.lua");

        WHEN("We call the plugin") {
            THEN("It should throw an exception") {
                REQUIRE_THROWS_AS(plugin.apply(Task(),
                                               VariablesMap("valgrind-test"),
                                               context),
                                  runtime_error);
            }
        }
    }
}
} // namespace execHelper::plugins::test
