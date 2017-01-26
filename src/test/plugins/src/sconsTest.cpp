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
using execHelper::test::utils::TargetUtilNames;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::CompilerUtilNames;
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

            Scons plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("random-command", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"scons", "--jobs", "8"}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
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

            Scons plugin;

            WHEN("We build the plugin") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"scons", "--jobs", "8"}));
                    expectedQueue.push_back(expectedTask);

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
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

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the single-threaded option of the scons plugin", "[plugins][scons]") {
        OptionsStub options;
        setupBasicOptions(options, {});

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

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
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

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
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

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
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

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
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
                        const CompilerUtilNames compilerNames = compilerUtil.toNames(pattern);

                        for(const auto& target : targetUtil.makePatternPermutator()) {
                            const TargetUtilNames targetNames = targetUtil.toNames(target);

                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "--jobs", "8", "compiler=" + compilerNames.compiler, "mode=" + compilerNames.mode, compilerNames.architecture, "hello" + compilerNames.distribution + "world", targetNames.target + targetNames.runTarget}));
                            expectedQueue.push_back(expectedTask);
                        }
                    }

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
            WHEN("We clean the plugin") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& pattern : compilerUtil.makePatternPermutator()) {
                        const CompilerUtilNames compilerNames = compilerUtil.toNames(pattern);

                        for(const auto& target : targetUtil.makePatternPermutator()) {
                            const TargetUtilNames targetNames = targetUtil.toNames(target);
 
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "--jobs", "8", "--clean", "compiler=" + compilerNames.compiler, "mode=" + compilerNames.mode, compilerNames.architecture, "hello" + compilerNames.distribution + "world", targetNames.target + targetNames.runTarget}));
                            expectedQueue.push_back(expectedTask);
                        }
                    }

                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }
} } }
