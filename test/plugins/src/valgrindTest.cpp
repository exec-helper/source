#include <array>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <gsl/pointers>

#include "config/settingsNode.h"
#include "core/task.h"
#include "plugins/executePlugin.h"
#include "plugins/luaPlugin.h"
#include "plugins/memory.h"

#include "config/generators.h"
#include "core/coreGenerators.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/addToConfig.h"
#include "utils/addToTask.h"
#include "utils/commonGenerators.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"
#include "handlers.h"

namespace filesystem = std::filesystem;

using std::array;
using std::make_pair;
using std::make_shared;
using std::map;
using std::optional;
using std::runtime_error;
using std::shared_ptr;
using std::static_pointer_cast;
using std::string;
using std::string_view;
using std::vector;

using gsl::not_null;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

using execHelper::core::test::ExecutorStub;
using execHelper::test::addToConfig;
using execHelper::test::addToTask;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::propertyTest;

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
    propertyTest("", [](const optional<Tool>& tool,
                        const optional<filesystem::path>& workingDir,
                        const optional<vector<string>>& commandLine,
                        const optional<EnvironmentCollection>& environment,
                        const optional<bool> verbose, const Pattern& pattern,
                        const Task& task) {
        Task expectedTask = task;
        expectedTask.append("valgrind");

        VariablesMap config("valgrind-test");

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/valgrind.lua");

        map<std::string, shared_ptr<SpecialMemory>> memories;
        const auto& patternValues = pattern.getValues();
        transform(patternValues.begin(), patternValues.end(),
                  inserter(memories, memories.end()), [](const auto& value) {
                      return make_pair(value, make_shared<SpecialMemory>());
                  });

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        auto runCommand = string("{").append(pattern.getKey()).append("}");
        addToConfig("run-command", runCommand, &config);

        addToConfig("tool", tool, &config);
        addToTask(tool, &expectedTask, [](const auto& tool) -> TaskCollection {
            return {std::string("--tool=").append(tool)};
        });

        if(verbose) {
            handleVerbosity(*verbose, "--verbose", config, expectedTask);
        }

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
        }

        if(commandLine) {
            handleCommandLine(*commandLine, config, expectedTask);
        }

        // Register each memories mapping as the endpoint for every target command
        Plugins plugins;
        transform(memories.begin(), memories.end(),
                  inserter(plugins, plugins.end()), [](const auto& memory) {
                      return make_pair(
                          memory.first,
                          static_pointer_cast<Plugin>(memory.second));
                  });

        FleetingOptionsStub fleetingOptions;

        ExecutePlugin::push(move(plugins));
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode("selector-test"));
        ExecutePlugin::push({pattern});

        THEN_WHEN("We apply the plugin") {
            bool returnCode = plugin.apply(task, config, {pattern});
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                for(const auto& memory : memories) {
                    auto executions = memory.second->getExecutions();
                    for(const auto& execution : executions) {
                        REQUIRE(execution.task == expectedTask);
                        REQUIRE(execution.patterns.empty());
                    }
                }
            }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
        ExecutePlugin::popPlugins();
    });
}

SCENARIO("Test erroneous scenarios", "[valgrind]") {
    GIVEN("A configuration without a configured run command") {
        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/valgrind.lua");
        VariablesMap variables("valgrind-test");

        WHEN("We call the plugin") {
            bool returnCode = plugin.apply(Task(), variables, Patterns());

            THEN("It should fail") { REQUIRE_FALSE(returnCode); }
        }
    }
}
} // namespace execHelper::plugins::test
