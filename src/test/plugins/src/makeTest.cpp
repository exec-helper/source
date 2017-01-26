#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>

#include "plugins/pluginUtils.h"
#include "plugins/make.h"

#include "executorStub.h"
#include "optionsStub.h"

#include "utils/utils.h"

using std::vector;
using std::string;
using std::ofstream;

using execHelper::core::test::ExecutorStub;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CommandCollection;
using execHelper::core::Pattern;
using execHelper::plugins::replacePatternCombinations;

using execHelper::test::OptionsStub;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::TargetUtilNames;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::CompilerUtilNames;
using execHelper::test::utils::Patterns;

namespace {
    void setupBasicOptions(OptionsStub& options, const Patterns& patterns) {
        addSettings(options.m_settings, "commands", {"build", "clean"});
        addSettings(options.m_settings, "build", {"make"});
        addSettings(options.m_settings, "clean", {"make"});

        for(const auto& pattern : patterns) {
            options.m_patternsHandler->addPattern(pattern);
        }
    }
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the default make settings", "[plugins][make]") {
        GIVEN("A make plugin object and the default options") {
            TargetUtil targetUtil;
            CompilerUtil compilerUtil;
            OptionsStub options;
            Patterns patterns;
            for(const auto& pattern : targetUtil.getPatterns()) {
                patterns.push_back(pattern);
            }
            for(const auto& pattern : compilerUtil.getPatterns()) {
                patterns.push_back(pattern);
            }
            setupBasicOptions(options, patterns);

            Make plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("random-command", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;

                Task expectedTask;
                expectedTask.append(TaskCollection({"make", "--jobs", "8"}));
                expectedQueue.push_back(expectedTask);

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the make multi-threaded setting", "[plugins][make]") {
        TargetUtil targetUtil;
        CompilerUtil compilerUtil;
        OptionsStub options;
        Patterns patterns;
        vector<string> patternNames;
        for(const auto& pattern : targetUtil.getPatterns()) {
            patterns.push_back(pattern);
            patternNames.push_back(pattern.getKey());
        }
        for(const auto& pattern : compilerUtil.getPatterns()) {
            patterns.push_back(pattern);
            patternNames.push_back(pattern.getKey());
        }
        setupBasicOptions(options, patterns);
        addSettings(options.m_settings["make"], "patterns", patternNames);
        addSettings(options.m_settings["make"], "command-line", {"{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}"});
        addSettings(options.m_settings["make"], "clean", "command-line");
        addSettings(options.m_settings["make"]["clean"], "command-line", {"clean", "{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}"});

        Make plugin;

        GIVEN("A make plugin object and several single-threaded option values") {
            WHEN("We build the plugin") {
                Task actualTaskBase({"make"});
                THEN("As a general command") {
                    AND_WHEN("If we set the single-threaded option") {
                        addSettings(options.m_settings["make"], "single-threaded", "yes");
                    }
                    AND_WHEN("If we set the multi-threaded option") {
                        addSettings(options.m_settings["make"], "single-threaded", "no");
                        actualTaskBase.append(TaskCollection{"--jobs", "8"});
                    }

                    Task task;
                    REQUIRE(plugin.apply("build", task, options) == true);
                }
                THEN("As a specific command") {
                    AND_WHEN("If we set the single-threaded option") {
                        addSettings(options.m_settings["make"], "single-threaded", "yes");
                    }
                    AND_WHEN("If we set the multi-threaded option") {
                        addSettings(options.m_settings["make"], "single-threaded", "no");
                        actualTaskBase.append(TaskCollection{"--jobs", "8"});
                    }
                    actualTaskBase.append("clean");

                    Task task;
                    REQUIRE(plugin.apply("clean", task, options) == true);
                }

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& pattern : compilerUtil.makePatternPermutator()) {
                    for(const auto& target : targetUtil.makePatternPermutator()) {
                        const TargetUtilNames targetNames = targetUtil.toNames(target);

                        Task expectedTask = actualTaskBase;
                        expectedTask.append(targetNames.target + targetNames.runTarget);
                        expectedQueue.push_back(expectedTask);
                    }
                }
                REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
            }
        }
    }

    SCENARIO("Testing the command-line make settings", "[plugins][make]") {
        GIVEN("A make plugin object and the default options") {
            TargetUtil targetUtil;
            CompilerUtil compilerUtil;
            OptionsStub options;
            Patterns patterns;
            vector<string> patternNames;
            for(const auto& pattern : targetUtil.getPatterns()) {
                patterns.push_back(pattern);
                patternNames.push_back(pattern.getKey());
            }
            for(const auto& pattern : compilerUtil.getPatterns()) {
                patterns.push_back(pattern);
            }

            setupBasicOptions(options, patterns);
            addSettings(options.m_settings["make"], "patterns", patternNames);
            addSettings(options.m_settings["make"], "clean", "command-line");
            addSettings(options.m_settings["make"], "command-line", {"V=1", "--jobs", "4", "{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}"});
            addSettings(options.m_settings["make"]["clean"], "command-line", {"clean", "V=1", "--jobs", "4", "{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}"});

            Make plugin;

            WHEN("We build the plugin") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& target : targetUtil.makePatternPermutator()) {
                    const TargetUtilNames targetNames = targetUtil.toNames(target);

                    Task expectedTask;
                    expectedTask.append(TaskCollection({"make", "--jobs", "8", "V=1", "--jobs", "4", targetNames.target + targetNames.runTarget}));
                    expectedQueue.push_back(expectedTask);
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }

            WHEN("We clean the plugin") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& target : targetUtil.makePatternPermutator()) {
                    const TargetUtilNames targetNames = targetUtil.toNames(target);

                    Task expectedTask;
                    expectedTask.append(TaskCollection({"make", "--jobs", "8", "clean", "V=1", "--jobs", "4", targetNames.target + targetNames.runTarget}));
                    expectedQueue.push_back(expectedTask);
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the build-dir make settings", "[plugins][make]") {
        GIVEN("A make plugin object and the default options") {
            const string command1("command1");
            const string command2("command2");

            TargetUtil targetUtil;
            CompilerUtil compilerUtil;
            OptionsStub options;
            Patterns patterns;
            vector<string> patternNames;
            for(const auto& pattern : targetUtil.getPatterns()) {
                patterns.push_back(pattern);
                patternNames.push_back(pattern.getKey());
            }
            for(const auto& pattern : compilerUtil.getPatterns()) {
                patterns.push_back(pattern);
                patternNames.push_back(pattern.getKey());
            }

            const string buildDir1("{" + compilerUtil.distribution.getKey() + "}/{" + compilerUtil.architecture.getKey() + "}/{HELLO}/{" + compilerUtil.compiler.getKey() + "}/hello{" + compilerUtil.mode.getKey() + " }world"); 
            const string buildDir2("clean/{" + compilerUtil.distribution.getKey() + "}/{" + compilerUtil.architecture.getKey() + "}/{HELLO}/{" + compilerUtil.compiler.getKey() + "}/hello{" + compilerUtil.mode.getKey() + "}world"); 
            const string target1({"{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}"});
            const string target2({"clean/{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}"});

            setupBasicOptions(options, patterns);
            addSettings(options.m_settings, "commands", {command1, command2});
            addSettings(options.m_settings, command1, {"make"});
            addSettings(options.m_settings, command2, {"make"});
            addSettings(options.m_settings["make"], "patterns", patternNames);
            addSettings(options.m_settings["make"], "command-line", {target1});
            addSettings(options.m_settings["make"], command2, "command-line");
            addSettings(options.m_settings["make"][command2], "command-line", {command2, target1});
            addSettings(options.m_settings["make"], "build-dir", buildDir1);
            addSettings(options.m_settings["make"][command2], "build-dir", buildDir2);

            Make plugin;
            Task task;

            WHEN("We create the expected output") {
                bool returnCode = false;
                
                TaskCollection expectedTaskCollection({"make", "--jobs", "8"});

                AND_WHEN("We call the first command") {
                    returnCode = plugin.apply(command1, task, options);

                    expectedTaskCollection.push_back("--directory=" + buildDir1);
                    expectedTaskCollection.push_back(target1);
                }
                AND_WHEN("We call the second command") {
                    returnCode = plugin.apply(command2, task, options);

                    expectedTaskCollection.push_back("--directory=" + buildDir2);
                    expectedTaskCollection.push_back(command2);
                    expectedTaskCollection.push_back(target1);
                }

                THEN_CHECK("The call should succeed") {
                    REQUIRE(returnCode == true);
                }

                THEN_CHECK("We get the expected tasks") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& compiler : compilerUtil.makePatternPermutator()) {
                        for(const auto& target : targetUtil.makePatternPermutator()) {
                            TaskCollection replacedTaskCollection = expectedTaskCollection;

                            replacePatternCombinations(replacedTaskCollection, compiler);
                            replacePatternCombinations(replacedTaskCollection, target);

                            Task expectedTask;
                            expectedTask.append(replacedTaskCollection);
                            expectedQueue.push_back(expectedTask);
                        }
                    }
                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }
} } }
