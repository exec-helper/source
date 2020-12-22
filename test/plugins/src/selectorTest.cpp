#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "config/commandLineOptions.h"
#include "config/environment.h"
#include "config/pattern.h"
#include "config/patternsHandler.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/logger.h"
#include "plugins/luaPlugin.h"

#include "base-utils/nonEmptyString.h"
#include "config/generators.h"
#include "core/coreGenerators.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/addToConfig.h"
#include "utils/addToTask.h"
#include "utils/commonGenerators.h"

#include "fleetingOptionsStub.h"
#include "handlers.h"
#include "utils/utils.h"

using std::optional;
using std::runtime_error;
using std::string;
using std::string_view;

using execHelper::config::Command;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Pattern;
using execHelper::config::PatternKey;
using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::PatternValues;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;

using execHelper::test::addToConfig;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::propertyTest;
using execHelper::test::utils::registerValuesAsCommands;

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
        task.addPatterns(patterns);

        Plugins plugins;
        auto memories = registerValuesAsCommands(pattern.getValues(), &plugins);

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

        FleetingOptionsStub options;
        SettingsNode settings("selector-test");
        PatternsHandler patternsHandler(patterns);
        const ExecutionContext context(options, settings, patternsHandler,
                                       plugins);

        Tasks expectedTasks(pattern.getValues().size(), task);

        THEN_WHEN("We apply the plugin") {
            auto actualTasks = plugin.apply(task, config, context);

            THEN_CHECK("It called the right commands") {
                REQUIRE(actualTasks == expectedTasks);
            }
        }
    });
}

SCENARIO("Unconfigured target in selector", "[selector]") {
    GIVEN("A config without a defined target") {
        LuaPlugin plugin(scriptPath());

        FleetingOptionsStub options;
        SettingsNode settings("selector-test");
        PatternsHandler patternsHandler;
        Plugins plugins;
        const ExecutionContext context(options, settings, patternsHandler,
                                       plugins);

        WHEN("We call the plugin") {
            THEN("It should throw a runtime error") {
                REQUIRE_THROWS_AS(plugin.apply(Task(),
                                               VariablesMap("selector-test"),
                                               context),
                                  runtime_error);
            }
        }
    }
}
} // namespace execHelper::plugins::test
