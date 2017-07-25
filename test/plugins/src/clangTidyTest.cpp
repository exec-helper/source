#include <map>
#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/variablesMap.h"
#include "plugins/clangTidy.h"
#include "plugins/commandLine.h"
#include "plugins/plugin.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::string;

using gsl::czstring;

using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::plugins::COMMAND_LINE_KEY;
using execHelper::plugins::ExecuteCallback;
using execHelper::plugins::registerExecuteCallback;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::utils::getExpectedTasks;

namespace {
    const czstring<> PLUGIN_NAME = "clang-tidy";
    const czstring<> SOURCES_KEY = "sources";
    const czstring<> CHECKS_KEY = "checks";
    const czstring<> WARNING_AS_ERROR_KEY = "warning-as-errors";
    const czstring<> WARNING_AS_ERROR_INHERIT_KEY = "all";

    TaskCollection toChecks(const TaskCollection& taskCollection) noexcept {
        if(taskCollection.empty()) {
            return TaskCollection();
        }
        string result="-checks=";
        for(size_t i = 0; i < taskCollection.size() - 1; ++i) {
            result += taskCollection[i] + ",";
        }
        result += taskCollection.back();
        return TaskCollection({result});
    }

    TaskCollection toWarningAsError(const TaskCollection& taskCollection) noexcept {
        if(taskCollection.empty()) {
            return TaskCollection();
        }
        string result="-warnings-as-errors=";
        for(size_t i = 0; i < taskCollection.size() - 1; ++i) {
            result += taskCollection[i] + ",";
        }
        result += taskCollection.back();
        return TaskCollection({result});
    }
} // namespace

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Obtain the plugin name of the clang-tidy plugin", "[clang-tidy]") {
        GIVEN("A plugin") {
            ClangTidy plugin;

            WHEN("We request the plugin name") {
                const string pluginName = plugin.getPluginName();

                THEN("We should find the correct plugin name") {
                    REQUIRE(pluginName == PLUGIN_NAME);
                }
            }
        }
    }

    SCENARIO("Obtaining the default variables map of the clang-tidy plugin", "[clang-tidy]") {
        GIVEN("The default fleeting options") {
            FleetingOptionsStub fleetingOptions;
            ClangTidy plugin;

            VariablesMap actualVariables(plugin.getPluginName());
            actualVariables.add(COMMAND_LINE_KEY);
            actualVariables.add(SOURCES_KEY, "*.cpp");

            WHEN("We request the variables map") {
                VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

                THEN("We should find the same ones") {
                    REQUIRE(variables == actualVariables);
                }
            }
        }
    }

    SCENARIO("Testing the configuration settings of the clang-tidy plugin", "[clang-tidy]") {
        MAKE_COMBINATIONS("Of several settings") {
            const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
            const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
            const Patterns patterns({pattern1, pattern2});

            ClangTidy plugin;
            Task task;

            VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

            TaskCollection commandLine;
            TaskCollection sources = variables.get<TaskCollection>(SOURCES_KEY, {});
            TaskCollection checks;
            TaskCollection warningAsError;

            ExecutorStub executor;
            ExecuteCallback executeCallback = [&executor](const Task& task) { executor.execute(task);};
            registerExecuteCallback(executeCallback);

            COMBINATIONS("Add sources") {
                sources = {"source1/{" + pattern1.getKey() + "}", "source2", "source3/{" + pattern2.getKey() + "}"};
                variables.replace(SOURCES_KEY, sources);
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + pattern1.getKey() + "}", "{" + pattern2.getKey() + "}"};
                variables.add(COMMAND_LINE_KEY, commandLine);
            }

            COMBINATIONS("Add checks") {
                checks = {"check1", "check2-{" + pattern1.getKey() + "}"};
                variables.replace(CHECKS_KEY, checks);
            }

            COMBINATIONS("Add warning as error") {
                const TaskCollection warningAsErrorValue = {"warningAsError1", "warningAsError2", "warningAsError3"};
                variables.replace(WARNING_AS_ERROR_KEY, warningAsErrorValue);
                warningAsError.emplace_back("warningAsError1,warningAsError2,warningAsError3");
            }

            COMBINATIONS("Inherit warning as error") {
                variables.replace(WARNING_AS_ERROR_KEY, WARNING_AS_ERROR_INHERIT_KEY);
                warningAsError = checks;
            }

            Task expectedTask({PLUGIN_NAME});
            expectedTask.append(toChecks(checks));
            expectedTask.append(toWarningAsError(warningAsError));
            expectedTask.append(commandLine);
            expectedTask.append(sources);
            const ExecutorStub::TaskQueue expectedTasks = getExpectedTasks(expectedTask, patterns);

            THEN_WHEN("We apply the plugin") {
                bool returnCode = plugin.apply(task, variables, patterns);
                THEN_CHECK("It should succeed") {
                    REQUIRE(returnCode);
                }

                THEN_CHECK("It called the right commands") {
                    REQUIRE(expectedTasks == executor.getExecutedTasks());
                }
            }
        }
    }
} // namespace test
} // namespace plugins
} // namespace execHelper
