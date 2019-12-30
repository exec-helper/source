#include <algorithm>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

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

using std::for_each;
using std::optional;
using std::string;
using std::string_view;
using std::vector;

using execHelper::config::EnvArgs;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Path;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

using execHelper::core::test::ExecutorStub;
using execHelper::test::propertyTest;
using execHelper::test::utils::getExpectedTasks;

namespace filesystem = std::filesystem;

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the clang-tidy plugin",
         "[clang-tidy]") {
    propertyTest("", [](const optional<filesystem::path>& buildDir,
                        const optional<filesystem::path>& workingDir,
                        const optional<vector<filesystem::path>>& sources,
                        const optional<vector<string>>& checks,
                        const optional<vector<string>>& warningAsErrors,
                        const optional<vector<string>>& commandLine,
                        const optional<EnvironmentCollection>& environment) {
        const Task task;
        Task expectedTask(task);
        Patterns patterns;

        VariablesMap config("clang-tidy-test");

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/clang-tidy.lua");

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        expectedTask.append("clang-tidy");

        const string directoryOption("-p");
        if(buildDir) {
            REQUIRE(config.add("build-dir", buildDir->string()));
            expectedTask.append({directoryOption, buildDir->string()});
        } else {
            expectedTask.append({directoryOption, "."});
        }

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(checks) {
            string checkString{"-checks="};
            REQUIRE(config.add("checks", *checks));
            for(auto it = checks->begin(); it != checks->end(); ++it) {
                if(it != checks->begin()) {
                    checkString.append(",");
                }
                checkString.append(*it);
            }
            expectedTask.append(move(checkString));
        }

        if(warningAsErrors) {
            REQUIRE(config.add("warning-as-errors", *warningAsErrors));
            string warningAsErrorString{"-warning-as-errors="};
            for(auto it = warningAsErrors->begin();
                it != warningAsErrors->end(); ++it) {
                if(it != warningAsErrors->begin()) {
                    warningAsErrorString.append(",");
                }
                warningAsErrorString.append(*it);
            }
            expectedTask.append(move(warningAsErrorString));
        } else if(checks) {
            // Set warning-as-errors to auto and expect the checks to be mirrored
            REQUIRE(config.add("warning-as-errors", "auto"));
            string warningAsErrorString{"-warning-as-errors="};
            for(auto it = checks->begin(); it != checks->end(); ++it) {
                if(it != checks->begin()) {
                    warningAsErrorString.append(",");
                }
                warningAsErrorString.append(*it);
            }
            expectedTask.append(move(warningAsErrorString));
        }

        if(commandLine) {
            handleCommandLine(*commandLine, config, expectedTask);
        }

        if(sources) {
            if(sources->empty()) {
                REQUIRE(config.add("sources"));
            } else {
                for_each(sources->begin(), sources->end(),
                         [&expectedTask, &config](const auto& source) {
                             expectedTask.append(source.string());
                             REQUIRE(config.add("sources", source.string()));
                         });
            }
        } else {
            expectedTask.append("*.cpp");
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
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
