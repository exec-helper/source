#include <filesystem>
#include <optional>
#include <string>

#include "config/environment.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "plugins/luaPlugin.h"

#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/commonGenerators.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"
#include "handlers.h"

using std::optional;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

using execHelper::core::test::ExecutorStub;
using execHelper::test::propertyTest;
using execHelper::test::utils::getExpectedTasks;

namespace filesystem = std::filesystem;

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the bootstrap plugin",
         "[bootstrap]") {
    propertyTest("", [](const optional<filesystem::path>& filename,
                        const optional<filesystem::path>& workingDir,
                        const optional<std::vector<std::string>>& commandLine,
                        const optional<EnvironmentCollection>& environment) {
        const Task task;
        Task expectedTask(task);
        Patterns patterns;

        VariablesMap config("make-test");

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/bootstrap.lua");

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

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

        ExecutorStub::TaskQueue expectedTasks =
            getExpectedTasks(expectedTask, patterns);

        THEN_WHEN("We apply the plugin") {
            bool returnCode = plugin.apply(task, config, patterns);

            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == executor.getExecutedTasks());
            }
        }
    });
}

} // namespace execHelper::plugins::test
