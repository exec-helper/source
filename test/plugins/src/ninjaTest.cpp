#include <filesystem>
#include <optional>
#include <string>

#include "config/commandLineOptions.h"
#include "config/environment.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "fleetingOptionsStub.h"
#include "plugins/luaPlugin.h"

#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/commonGenerators.h"
#include "utils/utils.h"

#include "handlers.h"

using std::optional;
using std::string;
using std::to_string;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Jobs_t;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;

using execHelper::test::FleetingOptionsStub;
using execHelper::test::propertyTest;

namespace filesystem = std::filesystem;

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the ninja plugin", "[ninja]") {
    const FleetingOptionsStub options;
    const SettingsNode settings("ninja");
    const PatternsHandler patternsHandler;
    const Plugins plugins;
    const ExecutionContext context(options, settings, patternsHandler, plugins);

    propertyTest("", [&context](
                         const optional<filesystem::path>& buildDir,
                         const optional<filesystem::path>& workingDir,
                         const optional<std::vector<std::string>>& commandLine,
                         const optional<EnvironmentCollection>& environment,
                         const optional<bool> verbose,
                         const optional<Jobs_t> jobs) {
        const Task task;
        Task expectedTask(task);

        VariablesMap config("ninja-test");

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

        handleVerbosity(verbose ? *verbose : context.options().getVerbosity(),
                        "--verbose", config, expectedTask);

        expectedTask.append(
            {"-j", to_string(jobs.value_or(context.options().getJobs()))});
        if(jobs) {
            REQUIRE(config.add("jobs", to_string(*jobs)));
        }

        if(commandLine) {
            handleCommandLine(*commandLine, config, expectedTask);
        }

        THEN_WHEN("We apply the plugin") {
            auto actualTasks =
                luaPlugin(task, config, context,
                          std::string(PLUGINS_INSTALL_PATH) + "/ninja.lua");

            THEN_CHECK("It called the right commands") {
                REQUIRE(actualTasks == Tasks({expectedTask}));
            }
        }
    });
}
} // namespace execHelper::plugins::test
