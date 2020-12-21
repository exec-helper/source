#include <filesystem>
#include <string>
#include <vector>

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
using std::string;
using std::to_string;
using std::vector;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Jobs_t;
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
SCENARIO("Testing the configuration settings of the cppcheck plugin",
         "[cppcheck]") {
    FleetingOptionsStub options;
    SettingsNode settings("clang-tidy");
    Plugins plugins;
    PatternsHandler patternsHandler;
    const ExecutionContext context(options, settings, patternsHandler, plugins);

    propertyTest("", [&context](
                         const optional<vector<filesystem::path>>& srcDir,
                         const optional<vector<string>>& checks,
                         const optional<filesystem::path>& workingDir,
                         const optional<vector<string>>& commandLine,
                         const optional<EnvironmentCollection>& environment,
                         const optional<bool> verbose,
                         const optional<Jobs_t> jobs) {
        const Task task;
        Task expectedTask(task);

        VariablesMap config("cppcheck-test");

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/cppcheck.lua");

        expectedTask.append("cppcheck");

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
        }

        string enableString("--enable=");
        if(checks) {
            REQUIRE(config.add("enable-checks", *checks));
            for(auto it = checks->begin(); it != checks->end(); ++it) {
                if(it != checks->begin()) {
                    enableString.append(",");
                }
                enableString.append(*it);
            }
            expectedTask.append(enableString);
        } else {
            enableString.append("all");
            expectedTask.append(enableString);
        }

        if(verbose) {
            handleVerbosity(*verbose, "--verbose", config, expectedTask);
        }

        if(jobs) {
            REQUIRE(config.add("jobs", std::to_string(*jobs)));
            expectedTask.append({"-j", std::to_string(*jobs)});
        } else {
            const std::string defaultNumberOfJobs{"1"};
            expectedTask.append({"-j", defaultNumberOfJobs});
        }

        if(commandLine) {
            handleCommandLine(*commandLine, config, expectedTask);
        }

        if(srcDir) {
            REQUIRE(config.add("src-dir"));
            for_each(srcDir->begin(), srcDir->end(),
                     [&config, &expectedTask](const auto& src) {
                         REQUIRE(config.add("src-dir", src.string()));
                         expectedTask.append(src.string());
                     });
        } else {
            expectedTask.append(".");
        }

        THEN_WHEN("We apply the plugin") {
            auto actualTasks = plugin.apply(task, config, context);

            THEN_CHECK("It generated the expected tasks") {
                REQUIRE(Tasks({expectedTask}) == actualTasks);
            }
        }
    });
}
} // namespace execHelper::plugins::test
