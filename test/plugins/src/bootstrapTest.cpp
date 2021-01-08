#include <filesystem>
#include <optional>
#include <string>

#include "config/environment.h"
#include "config/pattern.h"
#include "config/patternsHandler.h"
#include "config/variablesMap.h"
#include "plugins/luaPlugin.h"

#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/commonGenerators.h"
#include "utils/utils.h"

#include "fleetingOptionsStub.h"
#include "handlers.h"

using std::optional;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;

using execHelper::test::FleetingOptionsStub;
using execHelper::test::propertyTest;

namespace filesystem = std::filesystem;

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the bootstrap plugin",
         "[bootstrap]") {
    FleetingOptionsStub options;
    SettingsNode settings("clang-tidy");
    Plugins plugins;
    PatternsHandler patternsHandler;
    const ExecutionContext context(options, settings, patternsHandler, plugins);

    propertyTest("", [&context](
                         const optional<filesystem::path>& filename,
                         const optional<filesystem::path>& workingDir,
                         const optional<std::vector<std::string>>& commandLine,
                         const optional<EnvironmentCollection>& environment) {
        const Task task;
        Task expectedTask(task);

        VariablesMap config("make-test");

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
        }

        if(filename) {
            REQUIRE(config.add("filename", filename->string()));
            expectedTask.append(filename->string());
        } else {
            expectedTask.append("bootstrap.sh");
        }

        if(commandLine) {
            handleCommandLine(*commandLine, config, expectedTask);
        }

        THEN_WHEN("We apply the plugin") {
            auto actualTasks =
                luaPlugin(task, config, context,
                          std::string(PLUGINS_INSTALL_PATH) + "/bootstrap.lua");

            THEN_CHECK("It generated the expected tasks") {
                REQUIRE(Tasks({expectedTask}) == actualTasks);
            }
        }
    });
}
} // namespace execHelper::plugins::test
