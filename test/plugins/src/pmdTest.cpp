#include <array>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/pointers>

#include "config/environment.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/luaPlugin.h"

#include "core/coreGenerators.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "utils/addToConfig.h"
#include "utils/addToTask.h"
#include "utils/commonGenerators.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "handlers.h"

namespace filesystem = std::filesystem;

using std::array;
using std::optional;
using std::runtime_error;
using std::string;
using std::vector;

using gsl::not_null;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

using execHelper::core::test::ExecutorStub;
using execHelper::test::addToConfig;
using execHelper::test::addToTask;
using execHelper::test::propertyTest;
using execHelper::test::utils::getExpectedTasks;

namespace {
const uint16_t defaultMinimumTokens = 80U;

enum class Tool { Cpd };

constexpr auto getAllTools() { return array<Tool, 1>({Tool::Cpd}); }

inline void addToConfig(const execHelper::config::SettingsKeys& key,
                        const Tool tool, not_null<VariablesMap*> config) {
    string command;
    switch(tool) {
    case Tool::Cpd:
        command = "cpd";
        break;
    default:
        throw runtime_error("Invalid mode");
    }
    if(!config->add(key, command)) {
        throw runtime_error("Failed to add key " + key.back() +
                            " with mode value to config");
    }
}

inline void addToTask(const filesystem::path& exe, const Tool tool,
                      not_null<execHelper::core::Task*> task) {
    string command;
    switch(tool) {
    case Tool::Cpd:
        command = "cpd";
        break;
    default:
        throw runtime_error("Invalid mode");
    }
    execHelper::test::addToTask(
        exe.string().append("-").append(command), task,
        [](const string& value) -> TaskCollection { return {value}; });
}
} // namespace

namespace rc {
template <> struct Arbitrary<Tool> {
    static auto arbitrary() -> Gen<Tool> {
        return gen::elementOf(getAllTools());
    };
};
} // namespace rc

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the pmd plugin", "[pmd]") {
    propertyTest("", [](const optional<filesystem::path>& exe,
                        const optional<Tool>& tool,
                        const optional<filesystem::path>& workingDir,
                        const optional<vector<string>>& commandLine,
                        const optional<EnvironmentCollection>& environment,
                        const optional<bool> verbose,
                        const optional<string>& language,
                        const optional<uint16_t>& minimumTokens,
                        const optional<vector<filesystem::path>>& files,
                        const Task& task) {
        Task expectedTask(task);
        Patterns patterns;

        VariablesMap config("pmd-test");

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/pmd.lua");

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        addToConfig("exec", exe, &config);
        addToConfig("tool", tool, &config);
        addToTask(exe.value_or("pmd"), tool.value_or(Tool::Cpd), &expectedTask);

        addToConfig("language", language, &config);
        addToTask(language, &expectedTask,
                  [](const auto& language) -> TaskCollection {
                      return {"--language", language};
                  });

        if(verbose) {
            handleVerbosity(*verbose, "-verbose", config, expectedTask);
        }

        if(tool.value_or(Tool::Cpd) == Tool::Cpd) {
            addToConfig("minimum-tokens", minimumTokens, &config);
            addToTask(
                minimumTokens, &expectedTask,
                [](const auto& minimumTokens) -> TaskCollection {
                    return {"--minimum-tokens", minimumTokens};
                },
                defaultMinimumTokens);

            addToConfig("files", files, &config);
            addToTask(files, &expectedTask,
                      [](const auto& file) -> TaskCollection {
                          return {"--files", file};
                      });
        }

        if(workingDir) {
            handleWorkingDirectory(*workingDir, config, expectedTask);
        }

        if(environment) {
            handleEnvironment(*environment, config, expectedTask);
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
