/**
 *@file Tests properties that each plugin should have
 */
#include <iostream>
#include <sstream>

#include "config/pattern.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/commandLineCommand.h"
#include "plugins/executePlugin.h"
#include "plugins/logger.h"
#include "plugins/luaPlugin.h"
#include "plugins/plugin.h"
#include "plugins/pluginUtils.h"

#include "core/coreGenerators.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"
#include "handlers.h"
#include "pluginsGenerators.h"

using namespace std;

using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::PatternValue;
using execHelper::config::PatternValues;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;
using execHelper::plugins::ApplyFunction;
using execHelper::plugins::Plugins;

using execHelper::test::FleetingOptionsStub;
using execHelper::test::propertyTest;

namespace {
using namespace std::literals;

constexpr std::string_view patternKey = "BLAAT"sv;
} // namespace

namespace execHelper::plugins::test {
SCENARIO("Check that all plugins are found") {
    GIVEN("The expected number of plugins") {
        constexpr auto expectedNbOfPlugins = 15U;

        WHEN("We request all plugins") {
            const auto plugins = discoverPlugins({PLUGINS_INSTALL_PATH});

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

    SettingsNode settings("clang-static-analyzer-test");
    PatternsHandler patterns;
    auto plugins = discoverPlugins({PLUGINS_INSTALL_PATH});
    plugins.try_emplace("memory"s, memoryFunction());
    patterns.addPattern(Pattern(std::string(patternKey), {"memory"}));
    const ExecutionContext context(options, settings, patterns, plugins);

    propertyTest(
        "Every call to a plugin must lead to at least one generated task",
        [&context](const ApplyFunction& apply) {
            VariablesMap variablesMap("generic_plugin");
            REQUIRE(variablesMap.add("command-line", "blaat"));
            REQUIRE(variablesMap.add("build-command", "memory"));
            REQUIRE(variablesMap.add("run-command", "memory"));
            REQUIRE(variablesMap.add("container", "blaat"));
            REQUIRE(variablesMap.add("targets", "memory"));

            THEN_WHEN("We apply the plugin") {
                auto actualTasks = apply(core::Task(), variablesMap, context);

                THEN_CHECK("It should return at least one task") {
                    REQUIRE_FALSE(actualTasks.empty());
                }
            }
        });
}

SCENARIO("A plugin must not alter the arguments before a given task") {
    FleetingOptionsStub options;
    options.m_commands.push_back("memory");

    SettingsNode settings("clang-static-analyzer-test");
    PatternsHandler patterns;
    auto plugins = discoverPlugins({PLUGINS_INSTALL_PATH});
    plugins.try_emplace("memory"s, memoryFunction());
    patterns.addPattern(Pattern(std::string(patternKey), {"memory"}));
    const ExecutionContext context(options, settings, patterns, plugins);

    propertyTest(
        "A plugin must not alter the arguments already in a given task",
        [&context](const ApplyFunction& apply, const Task& task) {
            RC_PRE(!task.getTask().empty());

            VariablesMap variablesMap("generic_plugin");
            REQUIRE(variablesMap.add("command-line", "blaat"));
            REQUIRE(variablesMap.add("build-command", "memory"));
            REQUIRE(variablesMap.add("run-command", "memory"));
            REQUIRE(variablesMap.add("container", "blaat"));
            REQUIRE(variablesMap.add("targets", "memory"));

            THEN_WHEN("We apply the plugin") {
                auto actualTasks = apply(task, variablesMap, context);

                THEN_CHECK("At least one task is returned") {
                    REQUIRE_FALSE(actualTasks.empty());
                }

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

//SCENARIO("Print the plugin summary", "[generic-plugin][success]") {
//propertyTest("A plugin", [](SummaryFunction func) {
//WHEN("We request the summary of the plugin") {
//auto summary = func();

//THEN("The summary must not be empty") { REQUIRE(!summary.empty()); }
//}
//});
//}
} // namespace execHelper::plugins::test
