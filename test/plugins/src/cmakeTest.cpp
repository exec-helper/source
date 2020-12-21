#include <algorithm>
#include <array>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "plugins/luaPlugin.h"

#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"

#include "utils/commonGenerators.h"
#include "utils/utils.h"

#include "fleetingOptionsStub.h"
#include "handlers.h"

using std::array;
using std::optional;
using std::pair;
using std::runtime_error;
using std::string;
using std::string_view;
using std::vector;

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

namespace {
enum class Mode { Generate, Build, Install };

constexpr auto getAllModes() {
    return array<Mode, 3>({Mode::Generate, Mode::Build, Mode::Install});
}
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
    FleetingOptionsStub options;
    SettingsNode settings("cmake-test");
    PatternsHandler patternsHandler;
    Plugins plugins;
    const ExecutionContext context(options, settings, patternsHandler, plugins);

    propertyTest(
        "",
        [&context](
            Mode mode, const optional<filesystem::path>& sourcePath,
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

            VariablesMap config("cmake-test");

            LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/cmake.lua");

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

            THEN_WHEN("We apply the plugin") {
                auto actualTasks = plugin.apply(task, config, context);

                THEN_CHECK("It generated the expected tasks") {
                    REQUIRE(Tasks({expectedTask}) == actualTasks);
                }
            }
        });
}

SCENARIO("Set a wrong mode", "[cmake]") {
    FleetingOptionsStub options;
    SettingsNode settings("cmake-test");
    PatternsHandler patternsHandler;
    Plugins plugins;
    const ExecutionContext context(options, settings, patternsHandler, plugins);

    propertyTest("", [&context](const string& mode) {
        RC_PRE(mode != "generate");
        RC_PRE(mode != "build");
        RC_PRE(mode != "install");

        const Task task;

        VariablesMap config("cmake-test");

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/cmake.lua");

        REQUIRE(config.add("mode", mode));

        THEN_WHEN("We apply the plugin") {
            THEN_CHECK("It throws a runtime error") {
                REQUIRE_THROWS_AS(plugin.apply(task, config, context),
                                  runtime_error);
            }
        }
    });
}
} // namespace execHelper::plugins::test
