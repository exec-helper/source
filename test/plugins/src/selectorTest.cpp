#include <algorithm>
#include <filesystem>
#include <map>
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
#include "core/coreGenerators.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/addToConfig.h"
#include "utils/addToTask.h"
#include "utils/commonGenerators.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"
#include "handlers.h"

using std::count;
using std::inserter;
using std::make_pair;
using std::make_shared;
using std::map;
using std::move;
using std::optional;
using std::shared_ptr;
using std::static_pointer_cast;
using std::string;
using std::string_view;
using std::transform;

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

using execHelper::test::addToConfig;
using execHelper::test::FleetingOptionsStub;
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
                        const Pattern& pattern, Task task) {
        Patterns patterns = {pattern};

        map<std::string, shared_ptr<SpecialMemory>> memories;
        const auto& patternValues = pattern.getValues();
        transform(patternValues.begin(), patternValues.end(),
                  inserter(memories, memories.end()), [](const auto& value) {
                      return make_pair(value, make_shared<SpecialMemory>());
                  });

        VariablesMap config("selector-test");

        auto target = string("{").append(pattern.getKey()).append("}");

        LuaPlugin plugin(scriptPath());
        addToConfig("targets", target, &config);

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, task);
        }

        if(environment) {
            handleEnvironment(*environment, config, task);
        }

        FleetingOptionsStub fleetingOptions;

        // Register each memories mapping as the endpoint for every target command
        Plugins plugins;
        transform(memories.begin(), memories.end(),
                  inserter(plugins, plugins.end()), [](const auto& memory) {
                      return make_pair(
                          memory.first,
                          static_pointer_cast<Plugin>(memory.second));
                  });

        ExecutePlugin::push(move(plugins));
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode("selector-test"));
        ExecutePlugin::push(Patterns(patterns));

        THEN_WHEN("We apply the plugin") {
            bool returnCode = plugin.apply(task, config, patterns);
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK(
                "The memories were called the expected number of times") {
                for(const auto& memory : memories) {
                    // Expected executions is 1 per memory, multiplied by the number of occurences of the pattern value in all the pattern values
                    size_t nbOfExpectedExecutions =
                        count(patternValues.begin(), patternValues.end(),
                              memory.first);
                    REQUIRE(memory.second->getExecutions().size() ==
                            nbOfExpectedExecutions);
                }
            }

            THEN_CHECK("It called the right commands") {
                for(const auto& memory : memories) {
                    auto executions = memory.second->getExecutions();
                    for(const auto& execution : executions) {
                        REQUIRE(execution.task == task);
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

SCENARIO("Unconfigured target in selector", "[selector]") {
    GIVEN("A config without a defined target") {
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
} // namespace execHelper::plugins::test
