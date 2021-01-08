#include <array>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/pointers>

#include "config/environment.h"
#include "config/patternsHandler.h"
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

#include "fleetingOptionsStub.h"
#include "handlers.h"

namespace filesystem = std::filesystem;

using std::array;
using std::optional;
using std::runtime_error;
using std::string;
using std::vector;

using gsl::not_null;

using execHelper::config::EnvironmentCollection;
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
    FleetingOptionsStub options;
    SettingsNode settings("clang-tidy");
    Plugins plugins;
    PatternsHandler patternsHandler;
    const ExecutionContext context(options, settings, patternsHandler, plugins);

    propertyTest("", [&context](
                         const optional<filesystem::path>& exe,
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

        VariablesMap config("pmd-test");

        addToConfig("exec", exe, &config);
        addToConfig("tool", tool, &config);
        addToTask(exe.value_or("pmd"), tool.value_or(Tool::Cpd), &expectedTask);

        addToConfig("language", language, &config);
        addToTask(language, &expectedTask,
                  [](const auto& language) -> TaskCollection {
                      return {"--language", language};
                  });

        handleVerbosity(verbose ? *verbose : context.options().getVerbosity(),
                        "-verbose", config, expectedTask);

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

        THEN_WHEN("We apply the plugin") {
            auto actualTasks =
                luaPlugin(task, config, context,
                          std::string(PLUGINS_INSTALL_PATH) + "/pmd.lua");

            THEN_CHECK("It generated the expected tasks") {
                REQUIRE(Tasks({expectedTask}) == actualTasks);
            }
        }
    });
}
} // namespace execHelper::plugins::test
