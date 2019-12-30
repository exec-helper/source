#include <filesystem>
#include <optional>
#include <string>

#include "config/commandLineOptions.h"
#include "config/environment.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/luaPlugin.h"

#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/commonGenerators.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "handlers.h"

using std::optional;
using std::string;
using std::to_string;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Jobs_t;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

using execHelper::core::test::ExecutorStub;
using execHelper::test::propertyTest;
using execHelper::test::utils::getExpectedTasks;

namespace filesystem = std::filesystem;

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the ninja plugin", "[ninja]") {
    propertyTest("", [](const optional<filesystem::path>& buildDir,
                        const optional<filesystem::path>& workingDir,
                        const optional<std::vector<std::string>>& commandLine,
                        const optional<EnvironmentCollection>& environment,
                        const optional<bool> verbose,
                        const optional<Jobs_t> jobs) {
        const Task task;
        Task expectedTask(task);
        Patterns patterns;

        VariablesMap config("ninja-test");

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/ninja.lua");

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        expectedTask.append("ninja");

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
        }

        const string directoryOption("-C");
        if(buildDir) {
            REQUIRE(config.add("build-dir", buildDir->string()));
            expectedTask.append({directoryOption, buildDir->string()});
        } else {
            expectedTask.append({directoryOption, "."});
        }

        if(verbose) {
            handleVerbosity(*verbose, "--verbose", config, expectedTask);
        }

        if(jobs) {
            REQUIRE(config.add("jobs", std::to_string(*jobs)));
            expectedTask.append({"-j", to_string(*jobs)});
        } else {
            const std::string defaultNumberOfJobs{"1"};
            expectedTask.append({"-j", defaultNumberOfJobs});
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
