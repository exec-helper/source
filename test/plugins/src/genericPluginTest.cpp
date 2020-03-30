/**
 *@file Tests properties that each plugin should have
 */
#include <filesystem>

#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"
#include "pluginsGenerators.h"

#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/commandLineCommand.h"
#include "plugins/commandPlugin.h"
#include "plugins/executePlugin.h"
#include "plugins/lcov.h"
#include "plugins/logger.h"
#include "plugins/luaPlugin.h"
#include "plugins/memory.h"
#include "plugins/plugin.h"
#include "plugins/pluginUtils.h"
#include "plugins/pmd.h"
#include "plugins/valgrind.h"

#include "core/coreGenerators.h"

using std::shared_ptr;

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::PatternValue;
using execHelper::config::PatternValues;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;
using execHelper::plugins::MemoryHandler;
using execHelper::plugins::Plugin;
using execHelper::plugins::Plugins;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::propertyTest;

namespace filesystem = std::filesystem;

namespace {
constexpr std::string_view patternKey{"BLAAT"};

auto getPlugins() noexcept -> Plugins {
    Plugins plugins{
        {"command-line-command",
         shared_ptr<Plugin>(new execHelper::plugins::CommandLineCommand())},
        {"memory", shared_ptr<Plugin>(new execHelper::plugins::Memory())},
        {"valgrind", shared_ptr<Plugin>(new execHelper::plugins::Valgrind())},
        {"pmd", shared_ptr<Plugin>(new execHelper::plugins::Pmd())},
        {"lcov", shared_ptr<Plugin>(new execHelper::plugins::Lcov())},
    };

    auto searchPaths = {PLUGINS_INSTALL_PATH};
    for(const auto& path : searchPaths) {
        for(const auto& entry : filesystem::directory_iterator(path)) {
            if(entry.is_regular_file() && entry.path().extension() == ".lua") {
                plugins.emplace(std::make_pair(
                    entry.path().stem(),
                    shared_ptr<const Plugin>(
                        new execHelper::plugins::LuaPlugin(entry))));
            }
        }
    }
    return plugins;
}
} // namespace

namespace execHelper::plugins::test {
SCENARIO("Test the pattern keyword for each plugin") {
    REQUIRE(Plugin::getPatternsKey() == "patterns");
}

SCENARIO("Every call to a plugin must lead to at least one registered task") {
    FleetingOptionsStub options;
    options.m_commands.push_back("memory");

    execHelper::plugins::ExecutePlugin::push(
        gsl::not_null<FleetingOptionsInterface*>(&options));

    Patterns patterns = {Pattern(std::string(patternKey), {"memory"})};
    execHelper::plugins::ExecutePlugin::push(Patterns(patterns));

    execHelper::plugins::ExecutePlugin::push(SettingsNode("test"));
    execHelper::plugins::ExecutePlugin::push(getPlugins());

    propertyTest(
        "Every call to a plugin must lead to at least one registered task",
        [&patterns](shared_ptr<const Plugin>&& plugin) {
            uint32_t nbOfRegisteredTasks = 0U;

            REQUIRE(plugin);

            registerExecuteCallback(
                [&nbOfRegisteredTasks](const core::Task& /*task*/) {
                    ++nbOfRegisteredTasks;
                });
            MemoryHandler memory;

            auto variablesMap = plugin->getVariablesMap(FleetingOptionsStub());
            REQUIRE(variablesMap.add("command-line", "blaat"));
            REQUIRE(variablesMap.add("build-command", "memory"));
            REQUIRE(variablesMap.add("run-command", "memory"));
            REQUIRE(variablesMap.add("patterns", std::string(patternKey)));

            THEN_WHEN("We apply the plugin") {
                bool result =
                    plugin->apply(core::Task(), variablesMap, patterns);

                THEN_CHECK("The call must succeed") { REQUIRE(result); }

                THEN_CHECK(
                    "The executor should have been called at least once") {
                    REQUIRE(nbOfRegisteredTasks +
                                memory.getExecutions().size() >
                            0U);
                }
            }
        });

    execHelper::plugins::ExecutePlugin::popPlugins();
    execHelper::plugins::ExecutePlugin::popSettingsNode();
    execHelper::plugins::ExecutePlugin::popPatterns();
    execHelper::plugins::ExecutePlugin::popFleetingOptions();
}

SCENARIO("A plugin must not alter the arguments before a given task") {
    FleetingOptionsStub options;
    options.m_commands.push_back("memory");

    execHelper::plugins::ExecutePlugin::push(
        gsl::not_null<FleetingOptionsInterface*>(&options));

    Patterns patterns = {Pattern(std::string(patternKey), {"memory"})};
    execHelper::plugins::ExecutePlugin::push(Patterns(patterns));

    execHelper::plugins::ExecutePlugin::push(SettingsNode("test"));
    execHelper::plugins::ExecutePlugin::push(getPlugins());

    propertyTest(
        "A plugin must not alter the arguments already in a given task",
        [&patterns](std::shared_ptr<const Plugin>&& plugin, const Task& task) {
            RC_PRE(!task.getTask().empty());
            ExecutorStub executor;
            ExecuteCallback executeCallback = [&executor](const Task& task) {
                executor.execute(task);
            };
            registerExecuteCallback(executeCallback);

            auto variablesMap = plugin->getVariablesMap(FleetingOptionsStub());
            REQUIRE(variablesMap.add("command-line", "blaat"));
            REQUIRE(variablesMap.add("build-command", "memory"));
            REQUIRE(variablesMap.add("run-command", "memory"));
            REQUIRE(variablesMap.add("patterns", std::string(patternKey)));

            MemoryHandler memory;

            THEN_WHEN("We apply the plugin") {
                bool result = plugin->apply(task, variablesMap, patterns);

                THEN_CHECK("The call must succeed") { REQUIRE(result); }

                Tasks executedTasks = executor.getExecutedTasks();
                auto memories = MemoryHandler::getExecutions();
                std::transform(memories.begin(), memories.end(),
                               std::back_inserter(executedTasks),
                               [](const auto& mem) { return mem.task; });

                THEN_CHECK("The arguments before the task must remain") {
                    auto task_remains = std::any_of(
                        executedTasks.begin(), executedTasks.end(),
                        [&task](const auto& executedTask) {
                            // Check that any of the executed tasks starts with the content of task
                            auto actual = executedTask.getTask().begin();
                            return std::all_of(
                                task.getTask().begin(), task.getTask().end(),
                                [&actual](const std::string& expected) {
                                    return (*actual++ == expected);
                                });
                        });
                    REQUIRE(task_remains);
                }
            }
        });

    execHelper::plugins::ExecutePlugin::popPlugins();
    execHelper::plugins::ExecutePlugin::popSettingsNode();
    execHelper::plugins::ExecutePlugin::popPatterns();
    execHelper::plugins::ExecutePlugin::popFleetingOptions();
}

SCENARIO("Print the plugin summary", "[generic-plugin][success]") {
    FleetingOptionsStub options;
    options.m_commands.push_back("memory");

    execHelper::plugins::ExecutePlugin::push(
        gsl::not_null<FleetingOptionsInterface*>(&options));

    Patterns patterns = {Pattern(std::string(patternKey), {"memory"})};
    execHelper::plugins::ExecutePlugin::push(Patterns(patterns));

    execHelper::plugins::ExecutePlugin::push(SettingsNode("test"));
    execHelper::plugins::ExecutePlugin::push(getPlugins());

    propertyTest("A plugin", [](std::shared_ptr<const Plugin>&& plugin) {
        WHEN("We request the summary of the plugin") {
            auto summary = plugin->summary();

            THEN("The summary must not be empty") { REQUIRE(!summary.empty()); }
        }
    });

    execHelper::plugins::ExecutePlugin::popPlugins();
    execHelper::plugins::ExecutePlugin::popSettingsNode();
    execHelper::plugins::ExecutePlugin::popPatterns();
    execHelper::plugins::ExecutePlugin::popFleetingOptions();
}
} // namespace execHelper::plugins::test
