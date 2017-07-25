#include <string>

#include <gsl/string_span>

#include "config/pattern.h"
#include "config/variablesMap.h"
#include "plugins/commandLine.h"
#include "plugins/cppcheck.h"
#include "plugins/threadedness.h"
#include "plugins/verbosity.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::string;
using std::to_string;

using gsl::czstring;

using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::plugins::JOBS_KEY;
using execHelper::plugins::VERBOSITY_KEY;

using execHelper::test::FleetingOptionsStub;
using execHelper::core::test::ExecutorStub;
using execHelper::test::utils::getExpectedTasks;

namespace {
    const czstring<> PLUGIN_NAME = "cppcheck";
    const czstring<> ENABLE_CHECKS_KEY = "enable-checks";
    const czstring<> SRC_DIR_KEY = "src-dir";
} // namespace

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Obtain the plugin name of the cppcheck plugin", "[cppcheck]") {
        GIVEN("A plugin") {
            Cppcheck plugin;

            WHEN("We request the plugin name") {
                const string pluginName = plugin.getPluginName();

                THEN("We should find the correct plugin name") {
                    REQUIRE(pluginName == PLUGIN_NAME);
                }
            }
        }
    }

    SCENARIO("Obtain the default variables map of the cppcheck plugin", "[cppcheck]") {
        MAKE_COMBINATIONS("The default fleeting options") {
            FleetingOptionsStub fleetingOptions;
            Cppcheck plugin;

            VariablesMap actualVariables(plugin.getPluginName());
            actualVariables.add(ENABLE_CHECKS_KEY, "all");
            actualVariables.add(COMMAND_LINE_KEY);
            actualVariables.add(SRC_DIR_KEY, ".");
            actualVariables.add(VERBOSITY_KEY, "no");
            actualVariables.add(JOBS_KEY, to_string(fleetingOptions.m_jobs));

            COMBINATIONS("Switch on verbosity") {
                fleetingOptions.m_verbose = true;
                actualVariables.replace(VERBOSITY_KEY, "yes");
            }

            COMBINATIONS("Switch on threadedness") {
                fleetingOptions.m_jobs = 6U;
                actualVariables.replace(JOBS_KEY, to_string(fleetingOptions.m_jobs));
            }

            THEN_WHEN("We request the variables map") {
                VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

                THEN_CHECK("We should find the same ones") {
                    REQUIRE(variables == actualVariables);
                }
            }
        }
    }
 

    SCENARIO("Testing the configuration settings of the cppcheck plugin", "[cppcheck]") {
        MAKE_COMBINATIONS("Of several settings") {
            FleetingOptionsStub fleetingOptions;

            const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
            const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
            const Patterns patterns({pattern1, pattern2});

            Cppcheck plugin;
            
            VariablesMap variables = plugin.getVariablesMap(FleetingOptionsStub());

            std::string enabledChecks("--enable=all");
            std::string srcDir(".");
            Jobs jobs = fleetingOptions.m_jobs;
            TaskCollection verbosity;
            TaskCollection commandLine;

            ExecutorStub executor;
            ExecuteCallback executeCallback = [&executor](const Task& task) { executor.execute(task);};
            registerExecuteCallback(executeCallback);

            COMBINATIONS("Change the enabled checks") {
                TaskCollection enabledChecksValue = {"warning", "style", "performance"}; 
                variables.replace(ENABLE_CHECKS_KEY, enabledChecksValue);
                enabledChecks = "--enable=warning,style,performance";
            }

            COMBINATIONS("Change the source dir") {
                srcDir = "{" + pattern1.getKey() + "}/{HELLO}/{" + pattern2.getKey() + "}";
                variables.replace(SRC_DIR_KEY, srcDir);
            }

            COMBINATIONS("Switch on verbosity") {
                variables.replace(VERBOSITY_KEY, "yes");
                verbosity = {"--verbose"};
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + pattern1.getKey() + "}", "{" + pattern2.getKey() + "}"};
                variables.replace(COMMAND_LINE_KEY, commandLine);
            }

            COMBINATIONS("Switch on single threaded") {
                jobs = 1U;
                variables.replace(JOBS_KEY, to_string(jobs));
            }

            Task expectedTask({PLUGIN_NAME});
            expectedTask.append(enabledChecks);
            expectedTask.append(verbosity);
            expectedTask.append({"-j", to_string(jobs)});
            expectedTask.append(commandLine);
            expectedTask.append(srcDir);

            ExecutorStub::TaskQueue expectedTasks = getExpectedTasks(expectedTask, patterns);

            Task task;
            bool returnCode = plugin.apply(task, variables, patterns);
            THEN_CHECK("It should succeed") {
                REQUIRE(returnCode);
            }

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == executor.getExecutedTasks());
            }
        }
    }
} // namespace test
} // namespace plugins
} // namespace execHelper
