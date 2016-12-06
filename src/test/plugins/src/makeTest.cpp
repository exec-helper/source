#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>

#include "plugins/make.h"

#include "executorStub.h"
#include "optionsStub.h"
#include "core/compilerDescription.h"
#include "core/targetDescription.h"

#include "utils/utils.h"

using std::vector;
using std::string;
using std::ofstream;

using execHelper::core::test::ExecutorStub;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CompilerDescription;
using execHelper::core::TargetDescription;
using execHelper::core::CommandCollection;

using execHelper::test::OptionsStub;
using execHelper::test::utils::addSettings;

namespace {
    void setupBasicOptions(OptionsStub& options, const TargetDescription& targets, const CompilerDescription& compilers) {
        addSettings(options.m_settings, "commands", {"build", "clean"});
        addSettings(options.m_settings, "build", {"make"});
        addSettings(options.m_settings, "clean", {"make"});

        options.m_targets = targets;
        options.m_compilers = compilers;
    }
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the default make settings", "[plugins][make]") {
        GIVEN("A make plugin object and the default options") {
            const TargetDescription actualTargets({"target1", "target2"}, {"runTarget1", "runTarget2"});
            const CompilerDescription actualCompilers(CompilerDescription::CompilerNames({"compiler1", "compiler2"}), 
                                                                                         {"mode1", "mode2"}, 
                                                                                         {"architectureA", "architectureB"},
                                                                                         {"distribution1", "distribution2"}
                                                                                        );

            OptionsStub options;
            setupBasicOptions(options, actualTargets, actualCompilers);
            ExecutorStub executor;
            options.setExecutor(&executor);

            Make plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("random-command", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "--jobs", "8", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the make multi-threaded setting", "[plugins][make]") {
        const TargetDescription actualTargets({"target1", "target2"}, {"runTarget1", "runTarget2"});
        const CompilerDescription actualCompilers(CompilerDescription::CompilerNames({"compiler1", "compiler2"}), 
                                                                                     {"mode1", "mode2"}, 
                                                                                     {"architectureA", "architectureB"},
                                                                                     {"distribution1", "distribution2"}
                                                                                    );

        OptionsStub options;
        setupBasicOptions(options, actualTargets, actualCompilers);
        ExecutorStub executor;
        options.setExecutor(&executor);
        addSettings(options.m_settings["make"], "clean", "command-line");
        addSettings(options.m_settings["make"]["clean"], "command-line", "clean");
            
        Make plugin;

        GIVEN("A make plugin object and the single-threaded option") {
            addSettings(options.m_settings["make"], "single-threaded", "yes");

            WHEN("We build the plugin") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }

            WHEN("We clean the plugin") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "clean", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }

        GIVEN("A make plugin object and the multi-threaded option") {
            addSettings(options.m_settings["make"], "single-threaded", "no");

            WHEN("We build the plugin") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "--jobs", "8", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }

            WHEN("We clean the plugin") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "--jobs", "8", "clean", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the command-line make settings", "[plugins][make]") {
        GIVEN("A make plugin object and the default options") {
            const TargetDescription actualTargets({"target1", "target2"}, {"runTarget1", "runTarget2"});
            const CompilerDescription actualCompilers(CompilerDescription::CompilerNames({"compiler1", "compiler2"}), 
                                                                                         {"mode1", "mode2"}, 
                                                                                         {"architectureA", "architectureB"},
                                                                                         {"distribution1", "distribution2"}
                                                                                        );

            OptionsStub options;
            setupBasicOptions(options, actualTargets, actualCompilers);
            ExecutorStub executor;
            options.setExecutor(&executor);
            addSettings(options.m_settings["make"], "clean", "command-line");
            addSettings(options.m_settings["make"], "command-line", {"V=1", "--jobs", "4"});
            addSettings(options.m_settings["make"]["clean"], "command-line", {"clean", "V=1", "--jobs", "4"});

            Make plugin;

            WHEN("We build the plugin") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "--jobs", "8", "V=1", "--jobs", "4", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }

            WHEN("We clean the plugin") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "--jobs", "8", "clean", "V=1", "--jobs", "4", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the build-dir make settings", "[plugins][make]") {
        GIVEN("A make plugin object and the default options") {
            const TargetDescription actualTargets({"target1", "target2"}, {"runTarget1", "runTarget2"});
            const CompilerDescription actualCompilers(CompilerDescription::CompilerNames({"compiler1", "compiler2"}), 
                                                                                         {"mode1", "mode2"}, 
                                                                                         {"architectureA", "architectureB"},
                                                                                         {"distribution1", "distribution2"}
                                                                                        );

            OptionsStub options;
            setupBasicOptions(options, actualTargets, actualCompilers);
            ExecutorStub executor;
            options.setExecutor(&executor);
            addSettings(options.m_settings["make"], "clean", "command-line");
            addSettings(options.m_settings["make"]["clean"], "command-line", {"clean"});
            addSettings(options.m_settings["make"], "patterns", {"DISTRIBUTION", "ARCHITECTURE", "COMPILER", "MODE"});
            addSettings(options.m_settings["make"], "build-dir", "build/{DISTRIBUTION}/{ARCHITECTURE}/{HELLO}/{COMPILER}/hello{MODE}world");
            addSettings(options.m_settings["make"]["clean"], "build-dir", "clean/{DISTRIBUTION}/{ARCHITECTURE}/{HELLO}/{COMPILER}/hello{MODE}world");

            Make plugin;

            WHEN("We build the plugin") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    string compilerName = compiler.getCompiler().getName();
                    string modeName = compiler.getMode().getMode();
                    string architectureName = compiler.getArchitecture().getArchitecture();
                    string distributionName = compiler.getDistribution().getDistribution();

                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "--jobs", "8", "--directory=build/" + distributionName + "/" + architectureName + "/{HELLO}/" + compilerName + "/hello" + modeName + "world", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }

            WHEN("We clean the plugin") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    string compilerName = compiler.getCompiler().getName();
                    string modeName = compiler.getMode().getMode();
                    string architectureName = compiler.getArchitecture().getArchitecture();
                    string distributionName = compiler.getDistribution().getDistribution();

                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "--jobs", "8", "clean", "--directory=clean/" + distributionName + "/" + architectureName + "/{HELLO}/" + compilerName + "/hello" + modeName + "world", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }



    SCENARIO("Testing invalid make plugin commands", "[plugins][make]") {
        GIVEN("Nothing in particular") {
            OptionsStub options;

            WHEN("We pass an invalid command to scons") {
                Make plugin;
                Task task;

                THEN("It should fail") {
                    plugin.apply("blaat", task, options);
                }
            }
        }
    }
} } }
