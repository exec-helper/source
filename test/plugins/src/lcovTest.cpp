#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "config/environment.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/luaPlugin.h"

#include "config/generators.h"
#include "core/coreGenerators.h"
#include "unittest/catch.h"
#include "unittest/config.h"
#include "unittest/rapidcheck.h"
#include "utils/addToConfig.h"
#include "utils/addToTask.h"
#include "utils/commonGenerators.h"
#include "utils/utils.h"

#include "fleetingOptionsStub.h"
#include "handlers.h"

using std::optional;
using std::runtime_error;
using std::string;
using std::string_view;
using std::vector;
using std::filesystem::path;

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
using execHelper::test::propertyTest;
using execHelper::test::utils::getExpectedTasks;

namespace {
using namespace std::literals;
constexpr string_view binary = "lcov"sv;
inline auto defaultBaseDir() { return path("."sv); }
inline auto defaultDir() { return path("."sv); }
inline auto defaultInfoFile() { return path("lcov-plugin.info"sv); }
inline auto defaultGenHtmlOutput() { return path("."sv); }

auto zeroCounterTask(Task task, const optional<path>& workingDir,
                     const optional<vector<string>>& commandLine,
                     const optional<EnvironmentCollection>& environment,
                     const optional<path>& baseDirectory,
                     const optional<path>& directory) noexcept {
    task.append(string(binary));
    addToTask(
        baseDirectory, task,
        [](const auto& dir) -> TaskCollection {
            return {"--base-directory", dir};
        },
        defaultBaseDir());
    addToTask(
        directory, task,
        [](const auto& dir) -> TaskCollection {
            return {"--directory", dir};
        },
        defaultDir());
    task.append("--zerocounters");
    task.append(commandLine.value_or(vector<string>()));

    if(workingDir) {
        task.setWorkingDirectory(*workingDir);
    }
    if(environment) {
        task.setEnvironment(*environment);
    }
    return task;
}

auto captureTask(Task task, const optional<path>& workingDir,
                 const optional<vector<string>>& commandLine,
                 const optional<EnvironmentCollection>& environment,
                 const optional<path>& baseDirectory,
                 const optional<path>& directory,
                 const optional<path>& infoFile) noexcept {
    task.append(string(binary));
    addToTask(
        baseDirectory, task,
        [](const auto& dir) -> TaskCollection {
            return {"--base-directory", dir};
        },
        defaultBaseDir());
    addToTask(
        directory, task,
        [](const auto& dir) -> TaskCollection {
            return {"--directory", dir};
        },
        defaultDir());
    task.append("--capture");
    addToTask(
        infoFile, task,
        [](const auto& file) -> TaskCollection {
            return {"--output", file};
        },
        defaultInfoFile());
    task.append(commandLine.value_or(vector<string>()));

    if(workingDir) {
        task.setWorkingDirectory(*workingDir);
    }
    if(environment) {
        task.setEnvironment(*environment);
    }
    return task;
}

auto excludeTask(Task task, const optional<path>& workingDir,
                 const optional<vector<string>>& commandLine,
                 const optional<EnvironmentCollection>& environment,
                 const optional<path>& infoFile,
                 const vector<string>& excludes) noexcept {
    task.append({string(binary), "--remove"});
    addToTask(
        infoFile, task,
        [](const auto& file) -> TaskCollection { return {file}; },
        defaultInfoFile());
    addToTask(excludes, task, [](const auto& exclude) -> TaskCollection {
        return {"'" + exclude + "'"};
    });
    addToTask(
        infoFile, task,
        [](const auto& file) -> TaskCollection {
            return {"--output-file", file};
        },
        defaultInfoFile());
    task.append(commandLine.value_or(vector<string>()));

    if(workingDir) {
        task.setWorkingDirectory(*workingDir);
    }
    if(environment) {
        task.setEnvironment(*environment);
    }
    return task;
}

auto genHtmlTask(Task task, const optional<path>& workingDir,
                 const optional<EnvironmentCollection>& environment,
                 const optional<path>& infoFile,
                 const optional<path>& genHtmlOutput,
                 const optional<string>& genHtmlTitle,
                 const optional<vector<string>>& genHtmlCommandLine) noexcept {
    task.append("genhtml");
    addToTask(
        genHtmlOutput, task,
        [](const auto& output) -> TaskCollection {
            return {"--output-directory", output};
        },
        defaultGenHtmlOutput());
    addToTask(
        genHtmlTitle, task,
        [](const auto& title) -> TaskCollection {
            return {"--title", title};
        },
        "Hello"s);
    task.append(genHtmlCommandLine.value_or(vector<string>()));
    addToTask(
        infoFile, task,
        [](const auto& file) -> TaskCollection { return {file}; },
        defaultInfoFile());

    if(workingDir) {
        task.setWorkingDirectory(*workingDir);
    }
    if(environment) {
        task.setEnvironment(*environment);
    }
    return task;
}
} // namespace

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the lcov plugin", "[lcov]") {
    const FleetingOptionsStub options;
    const SettingsNode settings("lcov-test");

    propertyTest("", [&options, &settings](
                         const optional<path>& workingDir,
                         const optional<vector<string>>& commandLine,
                         const optional<EnvironmentCollection>& environment,
                         const optional<path>& baseDirectory,
                         const optional<path>& directory,
                         const optional<bool>& zeroCounters,
                         const optional<path>& infoFile,
                         const optional<vector<string>>& excludes,
                         const optional<bool>& genHtml,
                         const optional<path>& genHtmlOutput,
                         const optional<string>& genHtmlTitle,
                         const optional<vector<string>>& genHtmlCommandLine,
                         const Pattern& pattern, Task task) {
        VariablesMap config("lcov-test");
        task.addPatterns({pattern});

        Tasks expectedTasks;

        const auto plugins = mapToMemories(pattern.getValues());

        addToConfig("base-directory", baseDirectory, config);
        addToConfig("directory", directory, config);
        addToConfig("info-file", infoFile, config);
        addToConfig("command-line", commandLine, config);
        addToConfig("working-dir", workingDir, config);

        if(environment) {
            addToConfig("environment", vector<string>(), config);
            std::for_each(
                environment->begin(), environment->end(),
                [&config](const auto& env) {
                    REQUIRE(config.add({"environment", env.first}, env.second));
                });
        }

        addToConfig("zero-counters", zeroCounters, config);
        if(zeroCounters.value_or(false)) {
            auto zero = zeroCounterTask(task, workingDir, commandLine,
                                        environment, baseDirectory, directory);
            expectedTasks.emplace_back(zero);
        }

        auto runCommand = string("{").append(pattern.getKey()).append("}");
        addToConfig("run-command", runCommand, config);

        Task runTask = task;
        if(workingDir) {
            runTask.setWorkingDirectory(*workingDir);
        }
        if(environment) {
            runTask.setEnvironment(*environment);
        }
        auto replacedRunTasks = getExpectedTasks(runTask, {pattern});
        move(replacedRunTasks.begin(), replacedRunTasks.end(),
             back_inserter(expectedTasks));

        auto capture = captureTask(task, workingDir, commandLine, environment,
                                   baseDirectory, directory, infoFile);
        expectedTasks.emplace_back(capture);

        addToConfig("excludes", excludes, config);
        if(excludes && !excludes->empty()) {
            auto exclude = excludeTask(task, workingDir, commandLine,
                                       environment, infoFile, *excludes);
            expectedTasks.emplace_back(exclude);
        }

        addToConfig("gen-html", genHtml, config);
        addToConfig("gen-html-output", genHtmlOutput, config);
        addToConfig("gen-html-title", genHtmlTitle, config);
        addToConfig("gen-html-command-line", genHtmlCommandLine, config);

        if(genHtml.value_or(false)) {
            auto genHtml =
                genHtmlTask(task, workingDir, environment, infoFile,
                            genHtmlOutput, genHtmlTitle, genHtmlCommandLine);
            expectedTasks.emplace_back(genHtml);
        }

        PatternsHandler patternsHandler({pattern});
        const ExecutionContext context(options, settings, patternsHandler,
                                       plugins);

        THEN_WHEN("We apply the plugin") {
            auto actualTasks =
                luaPlugin(task, config, context,
                          std::string(PLUGINS_INSTALL_PATH) + "/lcov.lua");

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == actualTasks);
            }
        }
    });
}

SCENARIO("Test erroneous scenarios for the lcov plugin", "[lcov]") {
    GIVEN("A configuration without a configured run command") {
        VariablesMap variables("lcov-test");

        const FleetingOptionsStub options;
        const SettingsNode settings("lcov-test");
        const PatternsHandler patternsHandler;
        const Plugins plugins;
        const ExecutionContext context(options, settings, patternsHandler,
                                       plugins);

        WHEN("We call the plugin") {
            THEN("It should throw a runtime error") {
                REQUIRE_THROWS_AS(
                    luaPlugin(Task(), variables, context,
                              std::string(PLUGINS_INSTALL_PATH) + "/lcov.lua"),
                    runtime_error);
            }
        }
    }
}
} // namespace execHelper::plugins::test
