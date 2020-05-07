#include <algorithm>
#include <array>
#include <filesystem>
#include <optional>
#include <string_view>
#include <utility>

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

using std::array;
using std::optional;
using std::pair;
using std::string;
using std::string_view;
using std::vector;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Jobs_t;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

using execHelper::core::test::ExecutorStub;
using execHelper::test::propertyTest;
using execHelper::test::utils::getExpectedTasks;

namespace filesystem = std::filesystem;

namespace {
enum class Mode { Generate, Build, Install };

constexpr auto getAllModes() {
    return array<Mode, 3>({Mode::Generate, Mode::Build, Mode::Install});
};
} // namespace

namespace rc {
template <> struct Arbitrary<Mode> {
    static auto arbitrary() -> Gen<Mode> {
        return gen::elementOf(getAllModes());
    };
};
} // namespace rc

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the cmake plugin", "[cmake]") {
    propertyTest(
        "",
        [](Mode mode, const optional<filesystem::path>& sourcePath,
           const optional<filesystem::path>& buildPath,
           const optional<filesystem::path>& workingDir,
           const optional<vector<string>>& commandLine,
           const optional<EnvironmentCollection>& environment,
           const optional<bool> verbose, const optional<Jobs_t> jobs,
           const optional<string>& generator,
           const optional<pair<string, string>>&
               defines, // Problem: the order of a map is not necessarily preserved
           const optional<string>& target,
           const optional<filesystem::path>& prefix,
           const optional<string>& configuration,
           const optional<string>& component) {
            const Task task;
            Task expectedTask(task);
            Patterns patterns;

            VariablesMap config("cmake-test");

            LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/cmake.lua");

            ExecutorStub executor;
            ExecuteCallback executeCallback = [&executor](const Task& task) {
                executor.execute(task);
            };
            registerExecuteCallback(executeCallback);

            expectedTask.append("cmake");

            // Set all configuration options, even if they are not relevant
            if(sourcePath) {
                REQUIRE(config.add("source-dir", sourcePath->string()));
            }

            if(buildPath) {
                REQUIRE(config.add("build-dir", buildPath->string()));
            }

            if(generator) {
                REQUIRE(config.add("generator", *generator));
            }

            if(defines) {
                REQUIRE(
                    config.add({"defines", defines->first}, defines->second));
            }

            if(target) {
                REQUIRE(config.add("target", *target));
            }

            if(jobs) {
                REQUIRE(config.add("jobs", std::to_string(*jobs)));
            }

            if(prefix) {
                REQUIRE(config.add("prefix", prefix->string()));
            }

            if(configuration) {
                REQUIRE(config.add("configuration", *configuration));
            }

            if(component) {
                REQUIRE(config.add("component", *component));
            }

            switch(mode) {
            case Mode::Generate:
                REQUIRE(config.add("mode", "generate"));

                expectedTask.append(
                    {"-S", sourcePath ? sourcePath->string() : "."});
                expectedTask.append(
                    {"-B", buildPath ? buildPath->string() : "."});

                if(generator) {
                    expectedTask.append({"-G", *generator});
                }

                if(defines) {
                    expectedTask.append({"-D", string("\"") + defines->first +
                                                   "=" + defines->second +
                                                   "\""});
                }

                if(verbose) {
                    handleVerbosity(*verbose, "--log-level=VERBOSE", config,
                                    expectedTask);
                }
                break;
            case Mode::Build:
                REQUIRE(config.add("mode", "build"));

                expectedTask.append(
                    {"--build", buildPath ? buildPath->string() : "."});

                if(target) {
                    expectedTask.append({"--target", *target});
                }

                if(configuration) {
                    expectedTask.append({"--config", *configuration});
                }

                expectedTask.append(
                    {"--parallel", jobs ? std::to_string(*jobs) : "1"});

                if(verbose) {
                    handleVerbosity(*verbose, "--verbose", config,
                                    expectedTask);
                }
                break;
            case Mode::Install:
                REQUIRE(config.add("mode", "install"));

                expectedTask.append(
                    {"--install", buildPath ? buildPath->string() : "."});

                if(configuration) {
                    expectedTask.append({"--config", *configuration});
                }

                if(prefix) {
                    expectedTask.append({"--prefix", prefix->string()});
                }

                if(component) {
                    expectedTask.append({"--component", *component});
                }

                if(verbose) {
                    handleVerbosity(*verbose, "--verbose", config,
                                    expectedTask);
                }
                break;
            default:
                REQUIRE(false);
            }

            if(environment) {
                handleEnvironment(*environment, config, expectedTask);
            }

            if(commandLine) {
                handleCommandLine(*commandLine, config, expectedTask);
            }

            if(workingDir) {
                handleWorkingDirectory(*workingDir, config, expectedTask);
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

SCENARIO("Set a wrong mode", "[cmake]") {
    propertyTest("", [](const string& mode) {
        RC_PRE(mode != "generate");
        RC_PRE(mode != "build");
        RC_PRE(mode != "install");

        const Task task;

        Patterns patterns;
        VariablesMap config("cmake-test");

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/cmake.lua");

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        REQUIRE(config.add("mode", mode));

        THEN_WHEN("We apply the plugin") {
            bool returnCode = plugin.apply(task, config, patterns);

            THEN_CHECK("It should fail") { REQUIRE_FALSE(returnCode); }
        }
    });
}
} // namespace execHelper::plugins::test
