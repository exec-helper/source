#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>

#include "plugins/scons.h"

#include "executorStub.h"
#include "optionsStub.h"
#include "core/execHelperOptions.h"
#include "core/pattern.h"

#include "utils/utils.h"

using std::vector;
using std::string;
using std::ofstream;
using std::shared_ptr;
using std::make_shared;

using execHelper::config::SettingsNode;
using execHelper::core::ExecutorInterface;
using execHelper::core::test::ExecutorStub;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CommandCollection;
using execHelper::core::Pattern;
using execHelper::core::PatternKeys;

using execHelper::test::utils::addSettings;
using execHelper::test::OptionsStub;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::getAllPatterns;
using execHelper::test::utils::getAllPatternKeys;
using execHelper::test::utils::Patterns;

namespace {
    void setupBasicOptions(OptionsStub& options, const Patterns& patterns) {
        addSettings(options.m_settings, "commands", {"build", "clean"});
        addSettings(options.m_settings, "build", {"scons"});
        addSettings(options.m_settings, "clean", {"scons"});
        addSettings(options.m_settings["scons"], "clean", "command-line");
        addSettings(options.m_settings["scons"]["clean"], "command-line", {"--clean"});

        for(const auto& pattern : patterns) {
            options.m_patternsHandler->addPattern(pattern);
        }
    }

}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the default options of the scons plugin", "[plugins][scons]") {
        GIVEN("A scons plugin object and basic settings") {
            OptionsStub options;
            TargetUtil targetUtil;
            CompilerUtil compilerUtil;
            Patterns patterns = getAllPatterns({targetUtil, compilerUtil});
            setupBasicOptions(options, patterns);
            ExecutorStub executor;
            options.setExecutor(&executor);

            Scons plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("random-command", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"scons", "--jobs", "8"}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the all target of the scons plugin", "[plugins][scons]") {
        GIVEN("A scons plugin object, basic settings and the all target") {
            OptionsStub options;

            TargetUtil targetUtil;
            CompilerUtil compilerUtil;
            Patterns patterns = getAllPatterns({targetUtil, compilerUtil});
            setupBasicOptions(options, patterns);

            ExecutorStub executor;
            options.setExecutor(&executor);

            Scons plugin;

            WHEN("We build the plugin") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"scons", "--jobs", "8"}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
            WHEN("We clean the plugin") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"scons", "--jobs", "8", "--clean"}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the single-threaded option of the scons plugin", "[plugins][scons]") {
        OptionsStub options;
        setupBasicOptions(options, {});

        ExecutorStub executor;
        options.setExecutor(&executor);

        Scons plugin;

        GIVEN("A scons plugin object, basic settings and the single-threaded option") {
            addSettings(options.m_settings["scons"], "single-threaded", "yes");

            WHEN("We build the plugin wit the single threaded option") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"scons"}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
            WHEN("We clean the plugin") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"scons", "--clean"}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }

        GIVEN("A scons plugin object, basic settings and the multi-threaded option") {
            addSettings(options.m_settings["scons"], "single-threaded", "no");

            WHEN("We build the plugin wit the single threaded option") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"scons", "--jobs", "8"}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
            WHEN("We clean the plugin") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"scons", "--jobs", "8", "--clean"}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the command line option of the scons plugin", "[plugins][scons]") {
        GIVEN("A scons plugin object, basic settings and the multi-threaded option") {
            OptionsStub options;

            TargetUtil targetUtil;
            CompilerUtil compilerUtil;
            Patterns patterns = getAllPatterns({targetUtil, compilerUtil});
            PatternKeys patternKeys = getAllPatternKeys({targetUtil, compilerUtil});
            setupBasicOptions(options, patterns);

            ExecutorStub executor;
            options.setExecutor(&executor);

            vector<string> commandLine({"compiler={" + compilerUtil.compiler.getKey() + "}", "mode={" + compilerUtil.mode.getKey() + "}", "{" + compilerUtil.architecture.getKey() + "}", "hello{" + compilerUtil.distribution.getKey() + "}world", "{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}"});
            addSettings(options.m_settings["scons"], "patterns", patternKeys);
            addSettings(options.m_settings["scons"], "command-line", commandLine);
            addSettings(options.m_settings["scons"]["clean"], "command-line", commandLine);
        
            Scons plugin;

            WHEN("We build the plugin") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& pattern : compilerUtil.makePatternPermutator()) {
                        string compilerName = pattern.at(compilerUtil.compiler.getKey());
                        string modeName = pattern.at(compilerUtil.mode.getKey());
                        string architectureName = pattern.at(compilerUtil.architecture.getKey());
                        string distributionName = pattern.at(compilerUtil.distribution.getKey());

                        for(const auto& target : targetUtil.makePatternPermutator()) {
                            string targetName = target.at(targetUtil.target.getKey());
                            string runTargetName = target.at(targetUtil.runTarget.getKey());
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "--jobs", "8", "compiler=" + compilerName, "mode=" + modeName, architectureName, "hello" + distributionName + "world", targetName + runTargetName}));
                            expectedQueue.push_back(expectedTask);
                        }
                    }

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
            WHEN("We clean the plugin") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& pattern : compilerUtil.makePatternPermutator()) {
                        string compilerName = pattern.at(compilerUtil.compiler.getKey());
                        string modeName = pattern.at(compilerUtil.mode.getKey());
                        string architectureName = pattern.at(compilerUtil.architecture.getKey());
                        string distributionName = pattern.at(compilerUtil.distribution.getKey());

                        for(const auto& target : targetUtil.makePatternPermutator()) {
                            string targetName = target.at(targetUtil.target.getKey());
                            string runTargetName = target.at(targetUtil.runTarget.getKey());
 
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "--jobs", "8", "--clean", "compiler=" + compilerName, "mode=" + modeName, architectureName, "hello" + distributionName + "world", targetName + runTargetName}));
                            expectedQueue.push_back(expectedTask);
                        }
                    }

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }
} } }
