#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>

#include "plugins/cppcheck.h"

#include "executorStub.h"
#include "core/execHelperOptions.h"
#include "core/pattern.h"

#include "utils/utils.h"

#include "optionsStub.h"

using std::vector;
using std::string;
using std::ofstream;

using execHelper::core::test::ExecutorStub;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CommandCollection;
using execHelper::core::Pattern;

using execHelper::test::utils::addSettings;
using execHelper::test::OptionsStub;
using execHelper::test::utils::Patterns;

namespace {
    const string analyzeCommand("analyze");
    const string cppcheckCommand("cppcheck");
    void setupBasicOptions(OptionsStub& options, const Patterns& patterns = {}) {
        addSettings(options.m_settings, "commands", {analyzeCommand});
        addSettings(options.m_settings, analyzeCommand, {cppcheckCommand});

        for(const auto& pattern : patterns) {
            options.m_patternsHandler->addPattern(pattern);
        }
    }
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the default options of the cppcheck plugin", "[plugins][cppcheck]") {
        GIVEN("A cppcheck plugin object and basic settings") {
            OptionsStub options;
            setupBasicOptions(options);

            Cppcheck plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("analyze", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"cppcheck", "--enable=all", "."}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }
    SCENARIO("Testing the src-dir option of the cppcheck plugin", "[plugins][cppcheck]") {
        GIVEN("A cppcheck plugin object and a configuration") {
            const string srcDirKey("src-dir");
            const string srcDirValue("test-src");
            OptionsStub options;
            setupBasicOptions(options);
            addSettings(options.m_settings[cppcheckCommand], srcDirKey, srcDirValue);

            Cppcheck plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("analyze", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"cppcheck", "--enable=all", srcDirValue}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }
    SCENARIO("Testing the command-line option of the cppcheck plugin", "[plugins][cppcheck]") {
        GIVEN("A cppcheck plugin object and a configuration") {
            const string actualKey("command-line");
            const vector<string> actualValue({"--exit-error=1", "--std=c++11"});
            OptionsStub options;
            setupBasicOptions(options);
            addSettings(options.m_settings[cppcheckCommand], actualKey, actualValue);

            Cppcheck plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("analyze", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"cppcheck", "--enable=all"}));
                    for(const auto& value : actualValue) {
                        expectedTask.append(value);
                    }
                    expectedTask.append(".");
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the verbosity option of the cppcheck plugin", "[plugins][cppcheck]") {
        GIVEN("A cppcheck plugin object and a basic configuration") {
            OptionsStub options;
            setupBasicOptions(options);

            Cppcheck plugin;

            WHEN("We set the verbosity") {
                options.m_verbosity = true;

                Task task;
                bool returnCode = plugin.apply("analyze", task, options);

                THEN("It must succeed") {
                    REQUIRE(returnCode == true);
                }

                THEN("We should get the expected task") {
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"cppcheck", "--enable=all", "--verbose", "."}));
                    ExecutorStub::TaskQueue expectedQueue({expectedTask});

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the enable-checks option of the cppcheck plugin", "[plugins][cppcheck]") {
        GIVEN("A cppcheck plugin object and a configuration") {
            const string actualKey("enable-checks");
            const vector<string> actualValue({"style", "performance", "warning"});
            OptionsStub options;
            setupBasicOptions(options);
            addSettings(options.m_settings[cppcheckCommand], actualKey, actualValue);

            Cppcheck plugin;
            Task task;

            WHEN("We use the plugin") {

                REQUIRE(plugin.apply("analyze", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"cppcheck"}));
                    string enabledChecks("--enable=" + actualValue[0]);
                    for(size_t i = 1; i < actualValue.size(); ++i) {
                        enabledChecks += "," + actualValue[i];
                    }
                    expectedTask.append(enabledChecks);
                    expectedTask.append(".");
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }

        GIVEN("A cppcheck plugin object and an empty configuration") {
            const string actualKey("enable-checks");
            const vector<string> actualValue({});
            OptionsStub options;
            setupBasicOptions(options);
            addSettings(options.m_settings[cppcheckCommand], actualKey, actualValue);

            Cppcheck plugin;
            Task task;

            WHEN("We use the plugin") {

                REQUIRE(plugin.apply("analyze", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"cppcheck", "--enable=all"}));
                    expectedTask.append(".");
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }
    SCENARIO("Testing the target-path option of the cppcheck plugin", "[plugins][cppcheck]") {
        GIVEN("A cppcheck plugin object and a configuration") {
            Pattern targetPattern("TARGET", {"target"}, 't', "target");
            Pattern runTargetPattern("RUNTARGET", {"run-target"}, 'r', "run-target");

            OptionsStub options;
            setupBasicOptions(options, {targetPattern, runTargetPattern});
            addSettings(options.m_settings[cppcheckCommand], "patterns", {"TARGET", "RUNTARGET"});
            addSettings(options.m_settings[cppcheckCommand], "src-dir", "src/{TARGET}");
            addSettings(options.m_settings[cppcheckCommand], "target-path", "{TARGET}/{RUNTARGET}");

            Cppcheck plugin;
            Task task;

            WHEN("We use the plugin") {

                REQUIRE(plugin.apply("analyze", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"cppcheck", "--enable=all", "src/target/target/run-target"}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }
} } }
