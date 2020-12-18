/**
 *@file Tests properties that each plugin should have
 */
#include <filesystem>
#include <sstream>

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
#include "plugins/logger.h"
#include "plugins/luaPlugin.h"
#include "plugins/memory.h"
#include "plugins/plugin.h"
#include "plugins/pluginUtils.h"

#include "core/coreGenerators.h"
#include "unittest/catch.h"

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
using execHelper::plugins::Plugin;
using execHelper::plugins::Plugins;

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

SCENARIO("Check that all plugins are found") {
    GIVEN("The expected number of plugins") {
        constexpr auto expectedNbOfPlugins = 15U;

        WHEN("We request all plugins") {
            const auto plugins = getPlugins();

            THEN("The number of plugins must equal the expected number of "
                 "plugins") {
                REQUIRE(plugins.size() == expectedNbOfPlugins);
            }
        }
    }
}

SCENARIO("Every call to a plugin must lead to at least one registered task") {
    FleetingOptionsStub options;
    options.m_commands.push_back("memory");

    auto flRaii = execHelper::plugins::ExecutePlugin::push(
        gsl::not_null<FleetingOptionsInterface*>(&options));

    Patterns patterns = {Pattern(std::string(patternKey), {"memory"})};
    auto paRaii = execHelper::plugins::ExecutePlugin::push(Patterns(patterns));

    auto seRaii =
        execHelper::plugins::ExecutePlugin::push(SettingsNode("test"));
    auto plRaii = execHelper::plugins::ExecutePlugin::push(getPlugins());

    propertyTest(
        "Every call to a plugin must lead to at least one generated task",
        [](shared_ptr<const Plugin>&& plugin) {
            REQUIRE(plugin);

            auto variablesMap = plugin->getVariablesMap(FleetingOptionsStub());
            REQUIRE(variablesMap.add("command-line", "blaat"));
            REQUIRE(variablesMap.add("build-command", "memory"));
            REQUIRE(variablesMap.add("run-command", "memory"));
            REQUIRE(variablesMap.add("container", "blaat"));
            REQUIRE(variablesMap.add("targets", "memory"));

            THEN_WHEN("We apply the plugin") {
                auto actualTasks = plugin->apply(core::Task(), variablesMap);

                THEN_CHECK("It should return at least one task") {
                    REQUIRE_FALSE(actualTasks.empty());
                }
            }
        });
}

SCENARIO("A plugin must not alter the arguments before a given task") {
    FleetingOptionsStub options;
    options.m_commands.push_back("memory");

    auto seRaii =
        execHelper::plugins::ExecutePlugin::push(SettingsNode("test"));
    auto plRaii = execHelper::plugins::ExecutePlugin::push(getPlugins());
    auto flRaii = execHelper::plugins::ExecutePlugin::push(
        gsl::not_null<FleetingOptionsInterface*>(&options));

    Patterns patterns = {Pattern(std::string(patternKey), {"memory"})};
    auto paRaii = execHelper::plugins::ExecutePlugin::push(Patterns(patterns));

    propertyTest(
        "A plugin must not alter the arguments already in a given task",
        [](std::shared_ptr<const Plugin>&& plugin, const Task& task) {
            RC_PRE(!task.getTask().empty());

            auto variablesMap = plugin->getVariablesMap(FleetingOptionsStub());
            REQUIRE(variablesMap.add("command-line", "blaat"));
            REQUIRE(variablesMap.add("build-command", "memory"));
            REQUIRE(variablesMap.add("run-command", "memory"));
            REQUIRE(variablesMap.add("container", "blaat"));
            REQUIRE(variablesMap.add("targets", "memory"));

            THEN_WHEN("We apply the plugin") {
                auto actualTasks = plugin->apply(task, variablesMap);

                THEN_CHECK("The arguments before the task must remain") {
                    // At least one generated task must start with the input task
                    REQUIRE(any_of(
                        actualTasks.begin(), actualTasks.end(),
                        [&task](const auto& actualTask) {
                            // Check that the beginning of each executed tasks starts with the content of task
                            auto actual = actualTask.getTask().begin();
                            return std::all_of(
                                task.getTask().begin(), task.getTask().end(),
                                [&actual](const std::string& expected) {
                                    return (*actual++ == expected);
                                });
                        }));
                }
            }
        });
}

SCENARIO("Print the plugin summary", "[generic-plugin][success]") {
    auto plRaii = execHelper::plugins::ExecutePlugin::push(getPlugins());

    propertyTest("A plugin", [](std::shared_ptr<const Plugin>&& plugin) {
        WHEN("We request the summary of the plugin") {
            auto summary = plugin->summary();

            THEN("The summary must not be empty") { REQUIRE(!summary.empty()); }
        }
    });
}

SCENARIO("Stream the plugin summary", "[generic-plugin][success]") {
    auto plRaii = execHelper::plugins::ExecutePlugin::push(getPlugins());

    propertyTest("A plugin", [](std::shared_ptr<const Plugin>&& plugin) {
        THEN_WHEN("We request the summary of the plugin") {
            std::stringstream summary;
            summary << *plugin;

            THEN_CHECK("The summary must not be empty") {
                REQUIRE(!summary.str().empty());
            }

            THEN_CHECK("The summary must show the expected message") {
                REQUIRE(summary.str() == plugin->summary());
            }
        }
    });
}
} // namespace execHelper::plugins::test
