#include <vector>

#include <gsl/string_span>

#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/commandLine.h"
#include "plugins/pluginUtils.h"
#include "plugins/pmd.h"
#include "plugins/verbosity.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::plugins::COMMAND_LINE_KEY;
using execHelper::plugins::ExecuteCallback;
using execHelper::plugins::Pmd;
using execHelper::plugins::VERBOSITY_KEY;
using execHelper::plugins::registerExecuteCallback;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::utils::getExpectedTasks;

namespace {
const czstring<> PLUGIN_NAME = "pmd";
using Exec = string;
const czstring<> EXEC_KEY = "exec";
using Tool = string;
const czstring<> TOOL_KEY = "tool";
using MinimumTokens = string;
const czstring<> MINIMUM_TOKENS_KEY = "minimum-tokens";
using Files = vector<string>;
const czstring<> FILES_KEY = "files";
using Language = string;
const czstring<> LANGUAGE_KEY = "language";
} // namespace

namespace execHelper {
namespace plugins {
namespace test {
SCENARIO("Obtain the plugin name of the pmd plugin", "[pmd]") {
    GIVEN("A plugin") {
        Pmd plugin;

        WHEN("We request the plugin name") {
            const string pluginName = plugin.getPluginName();

            THEN("We should find the correct plugin name") {
                REQUIRE(pluginName == PLUGIN_NAME);
            }
        }
    }
}

SCENARIO("Obtaining the default variables map of the pmd plugin", "[pmd]") {
    MAKE_COMBINATIONS("The default fleeting options") {
        FleetingOptionsStub fleetingOptions;
        Pmd plugin;

        VariablesMap actualVariables(plugin.getPluginName());
        actualVariables.add(EXEC_KEY, PLUGIN_NAME);
        actualVariables.add(TOOL_KEY, "cpd");
        actualVariables.add(COMMAND_LINE_KEY);
        actualVariables.add(VERBOSITY_KEY, "no");

        COMBINATIONS("Switch on verbosity") {
            fleetingOptions.m_verbose = true;
            actualVariables.replace(VERBOSITY_KEY, "yes");
        }

        THEN_WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN_CHECK("We should find the same ones") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO("Make combinations of different configurations for the pmd plugin",
         "[pmd]") {
    MAKE_COMBINATIONS("Of several settings") {
        const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
        const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
        const Patterns patterns({pattern1, pattern2});

        Pmd plugin;
        Task task;

        VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

        Task expectedTask;
        TaskCollection minimumTokens;
        TaskCollection files;
        TaskCollection verbosity;
        TaskCollection language;
        TaskCollection commandLine;

        ExecutorStub executor;
        ExecuteCallback executeCallback = [&executor](const Task& task) {
            executor.execute(task);
        };
        registerExecuteCallback(executeCallback);

        COMBINATIONS("Switch exec") { variables.replace(EXEC_KEY, "exec.sh"); }

        COMBINATIONS("Switch the tool") {
            variables.replace(TOOL_KEY, "tool1");
        }

        COMBINATIONS("Switch on minimum tokens") {
            variables.replace(MINIMUM_TOKENS_KEY, "100");

            if(variables.get<Tool>(TOOL_KEY) == Tool("cpd")) {
                minimumTokens.emplace_back("--minimum-tokens");
                minimumTokens.emplace_back(
                    variables.get<MinimumTokens>(MINIMUM_TOKENS_KEY).get());
            }
        }

        COMBINATIONS("Switch on files") {
            const Files newFiles({"file1", "file2", "file*"});
            variables.replace(FILES_KEY, newFiles);

            if(variables.get<Tool>(TOOL_KEY) == Tool("cpd")) {
                for(auto file : newFiles) {
                    files.emplace_back("--files");
                    files.emplace_back(file);
                }
            }
        }

        COMBINATIONS("Add a language") {
            variables.replace(LANGUAGE_KEY, "language1");
            language.emplace_back("--language");
            language.push_back(variables.get<Language>(LANGUAGE_KEY).get());
        }

        COMBINATIONS("Add a command line") {
            commandLine = {"{" + pattern1.getKey() + "}",
                           "{" + pattern2.getKey() + "}"};
            variables.replace(COMMAND_LINE_KEY, commandLine);
        }

        COMBINATIONS("Switch on verbosity") {
            variables.replace(VERBOSITY_KEY, "yes");
            verbosity.emplace_back("-verbose");
        }

        expectedTask.append(variables.get<Exec>(EXEC_KEY).get());
        expectedTask.append(variables.get<Tool>(TOOL_KEY).get());
        expectedTask.append(language);
        expectedTask.append(verbosity);
        expectedTask.append(minimumTokens);
        expectedTask.append(files);
        expectedTask.append(commandLine);

        const ExecutorStub::TaskQueue expectedTasks =
            getExpectedTasks(expectedTask, patterns);

        bool returnCode = plugin.apply(task, variables, patterns);
        THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

        THEN_CHECK("It called the right commands") {
            REQUIRE(expectedTasks == executor.getExecutedTasks());
        }
    }
}
} // namespace test
} // namespace plugins
} // namespace execHelper
