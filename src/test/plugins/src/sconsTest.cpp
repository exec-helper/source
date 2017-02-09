#include <vector>
#include <string>
#include <fstream>

#include "unittest/catch.h"

#include "plugins/scons.h"

#include "executorStub.h"
#include "optionsStub.h"
#include "core/execHelperOptions.h"
#include "core/pattern.h"
#include "plugins/pluginUtils.h"

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
using execHelper::plugins::replacePatternCombinations;

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

    void initOptions(OptionsStub& options, const Patterns& patterns) {
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
        GIVEN("A basic setup") {
            OptionsStub options;
            initOptions(options, {});

            const string command("scons-command");

            addSettings(options.m_settings, "commands", command);
            addSettings(options.m_settings, "scons", command);

            Scons plugin;

            WHEN("We add various single threaded settings") {
                Task expectedTask({"scons"});

                AND_WHEN("We add no settings at all") {
                    expectedTask.append(TaskCollection({"--jobs", "8"}));
                }

                AND_WHEN("We add yes as a generic setting") {
                    addSettings(options.m_settings["scons"], "single-threaded", "yes");
                }

                AND_WHEN("We add yes as a specific setting") {
                    addSettings(options.m_settings["scons"][command], "single-threaded", "yes");
                }

                AND_WHEN("We add no as a generic setting") {
                    addSettings(options.m_settings["scons"], "single-threaded", "no");
                    expectedTask.append(TaskCollection({"--jobs", "8"}));
                }

                AND_WHEN("We add no as a specific setting") {
                    addSettings(options.m_settings["scons"][command], "single-threaded", "no");
                    expectedTask.append(TaskCollection({"--jobs", "8"}));
                }

                THEN_WHEN("We apply the plugin") {
                    Task task;

                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("The apply succeeded") {
                        REQUIRE(returnCode == true);
                    }

                    THEN_CHECK("We get the expected tasks") {
                        ExecutorStub::TaskQueue expectedQueue;
                        expectedQueue.emplace_back(expectedTask);

                        REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                    }

                }
            }
        }
    }

    SCENARIO("Testing the verbosity option of the scons plugin", "[plugins][scons]") {
        GIVEN("A basic setup") {
            OptionsStub options;
            initOptions(options, {});

            const string command("scons-command");

            addSettings(options.m_settings, "commands", command);
            addSettings(options.m_settings, "scons", command);

            Scons plugin;

            WHEN("We change the verbosity") {
                Task expectedTask({"scons", "--jobs", "8"});

                AND_WHEN("verbosity is set to false") {
                    options.m_verbosity = false;
                }

                AND_WHEN("verbosity is set to true") {
                    options.m_verbosity = true;
                    expectedTask.append("--debug=explain");
                }

                THEN_WHEN("We apply the plugin") {
                    Task task;

                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("The apply succeeded") {
                        REQUIRE(returnCode == true);
                    }

                    THEN_CHECK("We get the expected tasks") {
                        ExecutorStub::TaskQueue expectedQueue;
                        expectedQueue.emplace_back(expectedTask);

                        REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                    }

                }
            }
        }
    }



    SCENARIO("Testing the command line option of the scons plugin", "[plugins][scons]") {
        GIVEN("A basic setup") {
            OptionsStub options;

            const string command("scons-command");
            const TargetUtil targetUtil;
            const CompilerUtil compilerUtil;
            const Patterns patterns = getAllPatterns({targetUtil, compilerUtil});
            const PatternKeys patternKeys = getAllPatternKeys({targetUtil, compilerUtil});
            initOptions(options, patterns);

            addSettings(options.m_settings, "commands", command);
            addSettings(options.m_settings, "scons", command);

            vector<string> commandLine({"compiler={" + compilerUtil.compiler.getKey() + "}", "mode={" + compilerUtil.mode.getKey() + "}", "{" + compilerUtil.architecture.getKey() + "}", "hello{" + compilerUtil.distribution.getKey() + "}world", "{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}"});
        
            Scons plugin;

            WHEN("We apply the command line configuration setting") {
                Task expectedTask({"scons", "--jobs", "8"});

                addSettings(options.m_settings["scons"], "patterns", patternKeys);

                AND_WHEN("We pass no command line") {
                    ;
                }

                AND_WHEN("We add the command line as a generic setting") {
                    addSettings(options.m_settings["scons"], "command-line", commandLine);

                    expectedTask.append(commandLine);
                }

                AND_WHEN("We add the command line as a specific setting") {
                    addSettings(options.m_settings["scons"][command], "command-line", commandLine);

                    expectedTask.append(commandLine);
                }

                THEN_WHEN("We apply the plugin") {
                    Task task;
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("The apply succeeds") {
                        REQUIRE(returnCode == true);
                    }

                    THEN_CHECK("We get the expected task") {
                        ExecutorStub::TaskQueue expectedQueue;
                        for(const auto& compiler : compilerUtil.makePatternPermutator()) {
                            for(const auto& target : targetUtil.makePatternPermutator()) {
                                TaskCollection replacedTaskCollection = expectedTask.getTask();

                                replacePatternCombinations(replacedTaskCollection, compiler);
                                replacePatternCombinations(replacedTaskCollection, target);

                                Task replacedExpectedTask;
                                replacedExpectedTask.append(replacedTaskCollection);
                                expectedQueue.push_back(replacedExpectedTask);
                            }
                        }

                        REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                    }
                }
            }
        }
    }
} } }
