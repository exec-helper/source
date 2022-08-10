#include <algorithm>
#include <array>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>

#include "config/pattern.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/luaPlugin.h"

#include "config/generators.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"

#include "base-utils/nonEmptyString.h"
#include "utils/addToConfig.h"
#include "utils/addToTask.h"
#include "utils/commonGenerators.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"
#include "handlers.h"

using std::array;
using std::optional;
using std::pair;
using std::runtime_error;
using std::string;
using std::vector;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::Tasks;

using execHelper::test::addToConfig;
using execHelper::test::addToTask;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::NonEmptyString;
using execHelper::test::propertyTest;

namespace filesystem = std::filesystem;

namespace {
enum class Mode { Run, Exec };

constexpr auto getAllModes() { return array<Mode, 2>({Mode::Run, Mode::Exec}); }

inline void addToConfig(const execHelper::config::SettingsKeys& key,
                        const Mode mode, VariablesMap& config) {
    string command;
    switch(mode) {
    case Mode::Run:
        command = "run";
        break;
    case Mode::Exec:
        command = "exec";
        break;
    default:
        throw runtime_error("Invalid mode");
    }
    if(!config.add(key, command)) {
        throw runtime_error("Failed to add key " + key.back() +
                            " with mode value to config");
    }
}

inline void addToTask(const Mode mode, execHelper::core::Task& task) {
    string command;
    switch(mode) {
    case Mode::Run:
        command = "run";
        break;
    case Mode::Exec:
        command = "exec";
        break;
    default:
        throw runtime_error("Invalid mode");
    }
    execHelper::test::addToTask(
        command, task,
        [](const string& value) -> TaskCollection { return {value}; });
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
SCENARIO("Testing the configuration settings of the docker plugin",
         "[docker][successful]") {
    const FleetingOptionsStub options;
    const SettingsNode settings("general-docker");

    propertyTest(
        "",
        [&options, &settings](
            Mode mode, const NonEmptyString& image,
            const optional<filesystem::path>& workingDir,
            const optional<vector<string>>& commandLine,
            const optional<EnvironmentCollection>& environment,
            const NonEmptyString& container, const optional<bool>& interactive,
            const optional<bool>& tty, const optional<bool>& privileged,
            const optional<string>& user,
            const optional<pair<string, string>>&
                env, // Lua does not necessarily preserve the order of these, so we currently limit ourselves to one value
            const optional<vector<string>>& volumes, const Pattern& pattern) {
            Task task;

            Patterns patterns = {pattern};
            task.addPatterns(patterns);

            Task expectedTask = task;

            VariablesMap config("docker-test");

            SettingsNode generalSettings("docker-test");

            const auto plugins = mapToMemories(pattern.getValues());
            const PatternsHandler patternsHandler(patterns);
            const ExecutionContext context(options, settings, patternsHandler,
                                           plugins);

            expectedTask.append("docker");

            addToConfig("mode", mode, config);
            addToTask(mode, expectedTask);

            addToConfig("interactive", interactive, config);
            addToTask(
                interactive, expectedTask,
                [](const string& value) -> TaskCollection {
                    return {"--interactive=" + value};
                },
                false);

            addToConfig("tty", tty, config);
            addToTask(
                tty, expectedTask,
                [](const string& value) -> TaskCollection {
                    return {"--tty=" + value};
                },
                false);

            addToConfig("privileged", privileged, config);
            addToTask(
                privileged, expectedTask,
                [](const string& value) {
                    return value == "true" ? TaskCollection{"--privileged"}
                                           : TaskCollection();
                },
                false);

            addToConfig("user", user, config);
            addToTask(user, expectedTask,
                      [](const string& value) -> TaskCollection {
                          return {
                              "--user=\"" + value + "\"",
                          };
                      });

            addToConfig("env", env, config);
            addToTask(env, expectedTask,
                      [](const string& value) -> TaskCollection {
                          return {
                              "\"--env=" + value + "\"",
                          };
                      });

            if(commandLine) {
                handleCommandLine(*commandLine, config, expectedTask);
            }

            addToConfig("image", image, config);
            addToConfig("container", container, config);

            switch(mode) {
            case Mode::Run:
                addToConfig("volumes", volumes, config);
                addToTask(volumes, expectedTask,
                          [](const string& value) -> TaskCollection {
                              return {
                                  "\"--volume=" + value + "\"",
                              };
                          });

                expectedTask.append(*image);
                break;

            case Mode::Exec:
                expectedTask.append(*container);
                break;

            default:
                REQUIRE(false);
            }

            auto target = string("{").append(pattern.getKey()).append("}");
            addToConfig("targets", target, config);

            if(environment) {
                handleEnvironment(*environment, config, expectedTask);
            }

            if(workingDir) {
                handleWorkingDirectory(*workingDir, config, expectedTask);
            }

            THEN_WHEN("We apply the plugin") {
                auto actualTasks = luaPlugin(task, config, context,
                                             std::string(PLUGINS_INSTALL_PATH) +
                                                 "/docker.lua");

                THEN_CHECK("It generated the expected tasks") {
                    //Tasks expectedTasks( target ? 1U : 1U, expectedTask);
                    Tasks expectedTasks(pattern.getValues().size(),
                                        expectedTask);
                    REQUIRE(actualTasks == expectedTasks);
                }
            }
        });
}

SCENARIO("Not passing an image or container to the docker plugin",
         "[docker][error]") {
    const FleetingOptionsStub options;
    const SettingsNode settings("general-docker");
    const PatternsHandler patternsHandler;
    const Plugins plugins;
    const ExecutionContext context(options, settings, patternsHandler, plugins);

    propertyTest(
        "A docker plugin and a configuration configured for run mode",
        [&context](
            const Mode mode, const optional<bool>& interactive,
            const optional<bool>& tty, const optional<bool>& privileged,
            const optional<string>& user,
            const optional<pair<string, string>>&
                env, // Lua does not necessarily preserve the order of these, so we currently limit ourselves to one value
            const optional<vector<string>>& volumes) {
            VariablesMap config("docker-test");
            addToConfig("run", mode, config);
            addToConfig("interactive", interactive, config);
            addToConfig("tty", tty, config);
            addToConfig("privileged", privileged, config);
            addToConfig("user", user, config);
            addToConfig("env", env, config);
            addToConfig("volumes", volumes, config);

            THEN_WHEN("We call the docker plugin with this configuration") {
                THEN_CHECK("It throws a runtime error") {
                    REQUIRE_THROWS_AS(
                        luaPlugin(Task(), config, context,
                                  std::string(PLUGINS_INSTALL_PATH) +
                                      "/docker.lua"),
                        runtime_error);
                }
            }
        });
}
} // namespace execHelper::plugins::test
