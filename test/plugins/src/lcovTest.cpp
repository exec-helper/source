#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "config/environment.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/executePlugin.h"
#include "plugins/luaPlugin.h"
#include "plugins/memory.h"

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

using std::make_pair;
using std::make_shared;
using std::map;
using std::optional;
using std::shared_ptr;
using std::static_pointer_cast;
using std::string;
using std::string_view;
using std::vector;
using std::filesystem::path;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::plugins::ExecutePlugin;
using execHelper::plugins::LuaPlugin;

using execHelper::core::test::ExecutorStub;
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
                     const optional<path>& directory,
                     const Pattern& pattern) noexcept {
    task.append(string(binary));
    addToTask(
        baseDirectory, &task,
        [](const auto& dir) -> TaskCollection {
            return {"--base-directory", dir};
        },
        defaultBaseDir());
    addToTask(
        directory, &task,
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
    return getExpectedTasks(task, {pattern});
}

auto captureTask(Task task, const optional<path>& workingDir,
                 const optional<vector<string>>& commandLine,
                 const optional<EnvironmentCollection>& environment,
                 const optional<path>& baseDirectory,
                 const optional<path>& directory,
                 const optional<path>& infoFile,
                 const Pattern& pattern) noexcept {
    task.append(string(binary));
    addToTask(
        baseDirectory, &task,
        [](const auto& dir) -> TaskCollection {
            return {"--base-directory", dir};
        },
        defaultBaseDir());
    addToTask(
        directory, &task,
        [](const auto& dir) -> TaskCollection {
            return {"--directory", dir};
        },
        defaultDir());
    task.append("--capture");
    addToTask(
        infoFile, &task,
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
    return getExpectedTasks(task, {pattern});
}

auto excludeTask(Task task, const optional<path>& workingDir,
                 const optional<vector<string>>& commandLine,
                 const optional<EnvironmentCollection>& environment,
                 const optional<path>& infoFile, const vector<string>& excludes,
                 const Pattern& pattern) noexcept {
    task.append({string(binary), "--remove"});
    addToTask(
        infoFile, &task,
        [](const auto& file) -> TaskCollection { return {file}; },
        defaultInfoFile());
    addToTask(excludes, &task, [](const auto& exclude) -> TaskCollection {
        return {"'" + exclude + "'"};
    });
    addToTask(
        infoFile, &task,
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
    return getExpectedTasks(task, {pattern});
}

auto genHtmlTask(Task task, const optional<path>& workingDir,
                 const optional<EnvironmentCollection>& environment,
                 const optional<path>& infoFile,
                 const optional<path>& genHtmlOutput,
                 const optional<string>& genHtmlTitle,
                 const optional<vector<string>>& genHtmlCommandLine,
                 const Pattern& pattern) noexcept {
    task.append("genhtml");
    addToTask(
        genHtmlOutput, &task,
        [](const auto& output) -> TaskCollection {
            return {"--output-directory", output};
        },
        defaultGenHtmlOutput());
    addToTask(
        genHtmlTitle, &task,
        [](const auto& title) -> TaskCollection {
            return {"--title", title};
        },
        "Hello"s);
    task.append(genHtmlCommandLine.value_or(vector<string>()));
    addToTask(
        infoFile, &task,
        [](const auto& file) -> TaskCollection { return {file}; },
        defaultInfoFile());

    if(workingDir) {
        task.setWorkingDirectory(*workingDir);
    }
    if(environment) {
        task.setEnvironment(*environment);
    }
    return getExpectedTasks(task, {pattern});
}
} // namespace

namespace execHelper::plugins::test {
SCENARIO("Testing the configuration settings of the lcov plugin", "[lcov]") {
    propertyTest("", [](const optional<path>& workingDir,
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
                        const Pattern& pattern, const Task& task) {
        ExecutorStub::TaskQueue allTasks;

        VariablesMap config("lcov-test");

        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/lcov.lua");

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        map<std::string, shared_ptr<SpecialMemory>> memories;
        const auto& patternValues = pattern.getValues();
        transform(patternValues.begin(), patternValues.end(),
                  inserter(memories, memories.end()), [](const auto& value) {
                      return make_pair(value, make_shared<SpecialMemory>());
                  });

        addToConfig("base-directory", baseDirectory, &config);
        addToConfig("directory", directory, &config);
        addToConfig("info-file", infoFile, &config);
        addToConfig("command-line", commandLine, &config);
        addToConfig("working-dir", workingDir, &config);

        if(environment) {
            addToConfig("environment", vector<string>(), &config);
            std::for_each(
                environment->begin(), environment->end(),
                [&config](const auto& env) {
                    REQUIRE(config.add({"environment", env.first}, env.second));
                });
        }

        addToConfig("zero-counters", zeroCounters, &config);
        if(zeroCounters.value_or(false)) {
            auto replacedTasks =
                zeroCounterTask(task, workingDir, commandLine, environment,
                                baseDirectory, directory, pattern);
            allTasks.insert(allTasks.end(), replacedTasks.begin(),
                            replacedTasks.end());
        }

        auto runCommand = string("{").append(pattern.getKey()).append("}");
        addToConfig("run-command", runCommand, &config);

        Task runTask = task;
        if(workingDir) {
            runTask.setWorkingDirectory(*workingDir);
        }
        if(environment) {
            runTask.setEnvironment(*environment);
        }

        auto replacedTasks =
            captureTask(task, workingDir, commandLine, environment,
                        baseDirectory, directory, infoFile, pattern);
        allTasks.insert(allTasks.end(), replacedTasks.begin(),
                        replacedTasks.end());

        addToConfig("excludes", excludes, &config);
        if(excludes && !excludes->empty()) {
            auto excludeTasks =
                excludeTask(task, workingDir, commandLine, environment,
                            infoFile, *excludes, pattern);
            allTasks.insert(allTasks.end(), excludeTasks.begin(),
                            excludeTasks.end());
        }

        addToConfig("gen-html", genHtml, &config);
        addToConfig("gen-html-output", genHtmlOutput, &config);
        addToConfig("gen-html-title", genHtmlTitle, &config);
        addToConfig("gen-html-command-line", genHtmlCommandLine, &config);

        if(genHtml.value_or(false)) {
            auto genHtmlTasks = genHtmlTask(
                task, workingDir, environment, infoFile, genHtmlOutput,
                genHtmlTitle, genHtmlCommandLine, pattern);
            allTasks.insert(allTasks.end(), genHtmlTasks.begin(),
                            genHtmlTasks.end());
        }

        // Register each memories mapping as the endpoint for every target command
        Plugins plugins;
        transform(memories.begin(), memories.end(),
                  inserter(plugins, plugins.end()), [](const auto& memory) {
                      return make_pair(
                          memory.first,
                          static_pointer_cast<Plugin>(memory.second));
                  });

        FleetingOptionsStub fleetingOptions;

        ExecutePlugin::push(move(plugins));
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode("lcov-test"));
        ExecutePlugin::push({pattern});

        THEN_WHEN("We apply the plugin") {
            bool returnCode = plugin.apply(task, config, {pattern});
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                REQUIRE(allTasks == executor.getExecutedTasks());

                for(const auto& memory : memories) {
                    auto executions = memory.second->getExecutions();
                    for(const auto& execution : executions) {
                        REQUIRE(execution.task == runTask);
                        REQUIRE(execution.patterns.empty());
                    }
                }
            }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
        ExecutePlugin::popPlugins();
    });
}

SCENARIO("Test erroneous scenarios for the lcov plugin", "[lcov]") {
    GIVEN("A configuration without a configured run command") {
        LuaPlugin plugin(std::string(PLUGINS_INSTALL_PATH) + "/lcov.lua");
        VariablesMap variables("lcov-test");

        WHEN("We call the plugin") {
            auto returnCode = plugin.apply(Task(), variables, Patterns());

            THEN("It should fail") { REQUIRE_FALSE(returnCode); }
        }
    }
}
} // namespace execHelper::plugins::test
