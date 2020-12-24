#include <experimental/array>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <gsl/pointers>

#include "config/environment.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/executionContext.h"
#include "plugins/luaPlugin.h"

#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/addToConfig.h"
#include "utils/addToTask.h"
#include "utils/commonGenerators.h"

#include "core/coreGenerators.h"

#include "fleetingOptionsStub.h"
#include "handlers.h"

using namespace std;

using gsl::not_null;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Jobs_t;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::Tasks;

using execHelper::test::addToConfig;
using execHelper::test::addToTask;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::propertyTest;

namespace {
enum class Mode { Setup, Compile, Install, Test };
enum class BuildType { Plain, Debug, DebugOptimized, Release, MinSize, Custom };

constexpr auto getAllModes() {
    return std::experimental::make_array(Mode::Setup, Mode::Compile,
                                         Mode::Install, Mode::Test);
}
constexpr auto getAllBuildTypes() {
    return std::experimental::make_array(
        BuildType::Plain, BuildType::Debug, BuildType::DebugOptimized,
        BuildType::Release, BuildType::MinSize, BuildType::Custom);
}

inline auto modeToString(Mode mode) {
    switch(mode) {
    case Mode::Setup:
        return "setup"sv;
    case Mode::Compile:
        return "compile"sv;
    case Mode::Install:
        return "install"sv;
    case Mode::Test:
        return "test"sv;
    default:
        throw runtime_error("Invalid mode");
    }
}

inline void addToConfig(const execHelper::config::SettingsKey& key, Mode mode,
                        not_null<VariablesMap*> config) {
    auto command = modeToString(mode);
    if(!config->add(key, string(command))) {
        throw runtime_error("Failed to add key " + key +
                            " with mode value to config");
    }
}

inline void addToTask(Mode mode, gsl::not_null<execHelper::core::Task*> task) {
    auto command = modeToString(mode);
    execHelper::test::addToTask(
        command, task,
        [](const string& value) -> TaskCollection { return {value}; });
}

inline auto buildTypeToString(BuildType buildType) {
    switch(buildType) {
    case BuildType::Plain:
        return "plain"sv;
    case BuildType::Debug:
        return "debug"sv;
    case BuildType::DebugOptimized:
        return "debugoptimized"sv;
    case BuildType::Release:
        return "release"sv;
    case BuildType::MinSize:
        return "minsize"sv;
    case BuildType::Custom:
        return "custom"sv;
    default:
        throw runtime_error("Invalid build type");
    }
}

inline void addToConfig(const execHelper::config::SettingsKey& key,
                        BuildType buildType, not_null<VariablesMap*> config) {
    auto command = buildTypeToString(buildType);
    if(!config->add(key, string(command))) {
        throw runtime_error("Failed to add key " + key +
                            " with build type value to config");
    }
}

inline void addToTask(BuildType buildType,
                      gsl::not_null<execHelper::core::Task*> task) {
    auto command = buildTypeToString(buildType);
    execHelper::test::addToTask(command, task,
                                [](const string& value) -> TaskCollection {
                                    return {"--buildtype", value};
                                });
}
} // namespace

namespace rc {
template <> struct Arbitrary<Mode> {
    static auto arbitrary() { return gen::elementOf(getAllModes()); };
};

template <> struct Arbitrary<BuildType> {
    static auto arbitrary() { return gen::elementOf(getAllBuildTypes()); };
};
} // namespace rc

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the meson plugin",
         "[meson][successful]") {
    const FleetingOptionsStub options;
    const SettingsNode settings("general-meson");
    const PatternsHandler patterns;
    const Plugins plugins;
    const ExecutionContext context(options, settings, patterns, plugins);

    propertyTest(
        "", [&context](Mode mode, BuildType buildType, const Task& task,
                       const optional<filesystem::path>& workingDir,
                       const optional<vector<string>>& commandLine,
                       const optional<EnvironmentCollection>& environment,
                       const optional<filesystem::path>& sourceDir,
                       const optional<filesystem::path>& buildDir,
                       const optional<filesystem::path>& crossFile,
                       const optional<filesystem::path>& prefix,
                       const optional<pair<string, string>>& setupOptions,
                       const optional<vector<string>>& suites,
                       const optional<vector<string>>& targets,
                       const optional<Jobs_t> jobs) {
            Task expectedTask = task;

            VariablesMap config("meson-test");

            expectedTask.append("meson");

            addToConfig("mode", mode, &config);
            addToTask(mode, &expectedTask);

            addToConfig("build-type", buildType, &config);
            addToConfig("cross-file", crossFile, &config);
            addToConfig("prefix", prefix, &config);
            addToConfig("options", setupOptions, &config);
            if(mode == Mode::Setup) {
                addToTask(buildType, &expectedTask);
                addToTask(crossFile, &expectedTask,
                          [](const auto& path) -> TaskCollection {
                              return {"--cross-file", path};
                          });
                addToTask(prefix, &expectedTask,
                          [](const auto& path) -> TaskCollection {
                              return {"--prefix", path};
                          });
                addToTask(setupOptions, &expectedTask,
                          [](const auto& option) -> TaskCollection {
                              return {"-D", option};
                          });
            }

            addToConfig("jobs", jobs, &config);
            if(mode == Mode::Compile) {
                addToTask(
                    jobs, &expectedTask,
                    [](const auto& job) -> TaskCollection {
                        return {"--jobs", job};
                    },
                    context.options().getJobs());
            }

            addToConfig("suites", suites, &config);
            if(mode == Mode::Test) {
                addToTask(suites, &expectedTask,
                          [](const auto& suite) -> TaskCollection {
                              return {"--suite", suite};
                          });
            }

            addToConfig("build-dir", buildDir, &config);
            if(mode != Mode::Setup) {
                addToTask(
                    buildDir, &expectedTask,
                    [](const auto& path) -> TaskCollection {
                        return {"-C", path};
                    },
                    filesystem::path("."));
            }

            if(commandLine) {
                handleCommandLine(*commandLine, config, expectedTask);
            }

            addToConfig("targets", targets, &config);
            if(mode == Mode::Test) {
                addToTask(targets, &expectedTask,
                          [](const auto& target) -> TaskCollection {
                              return {target};
                          });
            }

            addToConfig("source-dir", sourceDir, &config);
            if(mode == Mode::Setup) {
                addToTask(
                    buildDir, &expectedTask,
                    [](const auto& path) -> TaskCollection { return {path}; },
                    filesystem::path("."));
                addToTask(
                    sourceDir, &expectedTask,
                    [](const auto& path) -> TaskCollection { return {path}; },
                    filesystem::path("."));
            }

            if(environment) {
                handleEnvironment(*environment, config, expectedTask);
            }

            if(workingDir) {
                handleWorkingDirectory(*workingDir, config, expectedTask);
            }

            THEN_WHEN("We apply the plugin") {
                auto actualTasks =
                    luaPlugin(task, config, context,
                              std::string(PLUGINS_INSTALL_PATH) + "/meson.lua");

                THEN_CHECK("It generated the expected tasks") {
                    REQUIRE(actualTasks == Tasks({expectedTask}));
                }
            }
        });
}

SCENARIO("Pass an invalid mode to the meson plugin", "[meson][error]") {
    const FleetingOptionsStub options;
    const SettingsNode settings("general-meson");
    const PatternsHandler patternsHandler;
    const Plugins plugins;
    const ExecutionContext context(options, settings, patternsHandler, plugins);

    propertyTest("", [&context](const Task& task, const std::string& mode) {
        RC_PRE(mode != "setup");
        RC_PRE(mode != "compile");
        RC_PRE(mode != "test");
        RC_PRE(mode != "install");

        VariablesMap config("meson-test");
        addToConfig("mode", mode, &config);

        THEN_WHEN("We call the meson plugin with this configuration") {
            THEN_CHECK("It throws a runtime error") {
                REQUIRE_THROWS_AS(
                    luaPlugin(task, config, context,
                              std::string(PLUGINS_INSTALL_PATH) + "/meson.lua"),
                    runtime_error);
            }
        }
    });
}
} // namespace execHelper::plugins::test
