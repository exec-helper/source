#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/path.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/executePlugin.h"
#include "plugins/lcov.h"
#include "plugins/memory.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "fleetingOptionsStub.h"

using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::CommandCollection;
using execHelper::config::Path;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::ExecutePlugin;
using execHelper::plugins::Lcov;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::utils::getExpectedTasks;

namespace {
const czstring<> PLUGIN_NAME = "lcov";
const czstring<> RUN_COMMAND = "run-command";
const czstring<> INFO_FILE_KEY = "info-file";
const czstring<> BASE_DIR_KEY = "base-directory";
const czstring<> DIR_KEY = "directory";
const czstring<> ZERO_COUNTERS_KEY = "zero-counters";
const czstring<> GEN_HTML_KEY = "gen-html";
const czstring<> GEN_HTML_OUTPUT_KEY = "gen-html-output";
const czstring<> GEN_HTML_TITLE_KEY = "gen-html-title";
const czstring<> GEN_HTML_COMMAND_LINE_KEY = "gen-html-command-line";
const czstring<> EXCLUDES_KEY = "excludes";
const czstring<> MEMORY_KEY = "memory";
} // namespace

namespace execHelper {
namespace plugins {
namespace test {
SCENARIO("Obtain the plugin name of the lcov plugin", "[lcov]") {
    GIVEN("A plugin") {
        Lcov plugin;

        WHEN("We request the plugin name") {
            const string pluginName = plugin.getPluginName();

            THEN("We should find the correct plugin name") {
                REQUIRE(pluginName == PLUGIN_NAME);
            }
        }
    }
}

SCENARIO("Obtaining the default variables map of the lcov plugin", "[lcov]") {
    GIVEN("The default fleeting options") {
        FleetingOptionsStub fleetingOptions;
        Lcov plugin;

        VariablesMap actualVariables(plugin.getPluginName());
        actualVariables.add(COMMAND_LINE_KEY, CommandLineArgs());
        actualVariables.add(INFO_FILE_KEY, "lcov-plugin.info");
        actualVariables.add(BASE_DIR_KEY, ".");
        actualVariables.add(DIR_KEY, ".");
        actualVariables.add(ZERO_COUNTERS_KEY, "no");
        actualVariables.add(GEN_HTML_KEY, "no");
        actualVariables.add(GEN_HTML_OUTPUT_KEY, ".");
        actualVariables.add(GEN_HTML_TITLE_KEY, "Hello");
        actualVariables.add(GEN_HTML_COMMAND_LINE_KEY, vector<string>());
        actualVariables.add(EXCLUDES_KEY, vector<string>());

        WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN("We should find the same ones") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO("Test multiple configurations of the lcov plugin", "[lcov]") {
    MAKE_COMBINATIONS("Of several configurations") {
        const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
        const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
        const Patterns patterns({pattern1, pattern2});

        Lcov plugin;
        Task task;

        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        CommandCollection runCommands({MEMORY_KEY});
        auto infoFile = variables.get<Path>(INFO_FILE_KEY).value();
        auto baseDir = variables.get<Path>(BASE_DIR_KEY).value();
        auto dir = variables.get<Path>(DIR_KEY).value();
        CommandLineArgs commandLine;
        bool zeroCounters = false;
        bool genHtml = false;
        Path genHtmlOutput = variables.get<Path>(GEN_HTML_OUTPUT_KEY).value();
        string genHtmlTitle("Hello");
        CommandLineArgs genHtmlCommandLine;
        vector<string> excludes;

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        FleetingOptionsStub fleetingOptions;
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode(PLUGIN_NAME));
        ExecutePlugin::push(Patterns(patterns));

        COMBINATIONS("Change the info file") {
            infoFile = "/tmp";
            variables.replace(INFO_FILE_KEY, infoFile.native());
        }

        COMBINATIONS("Change the base directory") {
            baseDir /= "tmp";
            variables.replace(BASE_DIR_KEY, baseDir.native());
        }

        COMBINATIONS("Change the directory") {
            dir /= "..";
            variables.replace(DIR_KEY, dir.native());
        }

        COMBINATIONS("Add a command line") {
            commandLine = {"{" + pattern1.getKey() + "}",
                           "{" + pattern2.getKey() + "}"};
            variables.add(COMMAND_LINE_KEY, commandLine);
        }

        COMBINATIONS("Switch on zero counters") {
            zeroCounters = true;
            variables.replace(ZERO_COUNTERS_KEY, "yes");
        }

        COMBINATIONS("Switch on html generation") {
            genHtml = true;
            variables.replace(GEN_HTML_KEY, "yes");
        }

        COMBINATIONS("Set html output") {
            genHtmlOutput = "/tmp/blaat";
            variables.replace(GEN_HTML_OUTPUT_KEY, genHtmlOutput.native());
        }

        COMBINATIONS("Set html title") {
            genHtmlTitle = "World!";
            variables.replace(GEN_HTML_TITLE_KEY, genHtmlTitle);
        }

        COMBINATIONS("Set gen html command line") {
            genHtmlCommandLine = {"{" + pattern1.getKey() + "}",
                                  "{" + pattern2.getKey() + "}"};
            variables.replace(GEN_HTML_COMMAND_LINE_KEY, genHtmlCommandLine);
        }

        COMBINATIONS("Set excludes") {
            excludes = {"exclude1", "{" + pattern1.getKey() + "}"};
            variables.replace(EXCLUDES_KEY, excludes);
        }

        ExecutorStub::TaskQueue expectedTasks;
        if(zeroCounters) {
            Task zeroCountersTask({PLUGIN_NAME, "--base-directory",
                                   baseDir.native(), "--directory",
                                   dir.native(), "--zerocounters"});
            zeroCountersTask.append(commandLine);
            expectedTasks.emplace_back(zeroCountersTask);
        }
        for(const auto& command : runCommands) {
            variables.add(RUN_COMMAND, command);
        }
        Task captureTask({PLUGIN_NAME, "--base-directory", baseDir.native(),
                          "--directory", dir.native(), "--capture", "--output",
                          infoFile.native()});
        captureTask.append(commandLine);
        expectedTasks.emplace_back(captureTask);

        if(!excludes.empty()) {
            Task excludeTask({PLUGIN_NAME, "--remove", infoFile.native()});
            for(const auto& exclude : excludes) {
                excludeTask.append(
                    string(R"(")").append(exclude).append(R"(")"));
            }
            excludeTask.append({"--output-file", infoFile.native()});
            excludeTask.append(commandLine);
            expectedTasks.emplace_back(excludeTask);
        }

        if(genHtml) {
            Task genHtml({"genhtml", "--output-directory",
                          genHtmlOutput.native(), "--title", genHtmlTitle});
            genHtml.append(genHtmlCommandLine);
            genHtml.append(infoFile.native());
            expectedTasks.emplace_back(genHtml);
        }

        const ExecutorStub::TaskQueue replacedTasks =
            getExpectedTasks(expectedTasks, patterns);

        THEN_WHEN("We apply the plugin") {
            bool returnCode = plugin.apply(task, variables, patterns);
            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                REQUIRE(replacedTasks == executor.getExecutedTasks());
            }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    }
}
} // namespace test
} // namespace plugins
} // namespace execHelper
