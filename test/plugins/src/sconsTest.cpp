#include <filesystem>
#include <string>
#include <string_view>

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
using std::string_view;
using std::to_string;

using execHelper::config::EnvArgs;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Jobs_t;
using execHelper::config::Path;
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
SCENARIO("Testing the configuration settings of the scons plugin", "[scons]") {
    FleetingOptionsStub options;
    SettingsNode settings("scons");
    PatternsHandler patternsHandler;
    Plugins plugins;
    const ExecutionContext context(options, settings, patternsHandler, plugins);

    propertyTest("", [&context](
                         const optional<filesystem::path>& workingDir,
                         const optional<filesystem::path>& buildDir,
                         const optional<std::vector<std::string>>& commandLine,
                         const optional<EnvironmentCollection>& environment,
                         const optional<bool> verbose,
                         const optional<Jobs_t> jobs) {
        const Task task;
        Task expectedTask(task);

        VariablesMap config("scons-test");

        expectedTask.append("scons");

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
        }

        const string directoryOption("--directory");
        if(buildDir) {
            REQUIRE(config.add("build-dir", buildDir->string()));
            expectedTask.append({directoryOption, buildDir->string()});
        } else {
            expectedTask.append({directoryOption, "."});
        }

        if(verbose) {
            handleVerbosity(*verbose, "--debug=explain", config, expectedTask);
        }

        if(jobs) {
            expectedTask.append({"--jobs", std::to_string(*jobs)});
            REQUIRE(config.add("jobs", std::to_string(*jobs)));
        } else {
            const std::string defaultNumberOfJobs{"1"};
            expectedTask.append({"--jobs", defaultNumberOfJobs});
        }

        if(commandLine) {
            handleCommandLine(*commandLine, config, expectedTask);
        }

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/scons.lua");

        THEN_WHEN("We apply the plugin") {
            auto actualTasks = plugin.apply(task, config, context);

            THEN_CHECK("It generated the expected tasks") {
                REQUIRE(Tasks{expectedTask} == actualTasks);
            }
        }
    });
}
} // namespace execHelper::plugins::test
