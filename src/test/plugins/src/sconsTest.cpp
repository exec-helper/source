#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>

#include "plugins/scons.h"

#include "executorStub.h"
#include "optionsStub.h"
#include "core/execHelperOptions.h"
#include "core/compilerDescription.h"
#include "core/targetDescription.h"

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
using execHelper::core::CompilerDescription;
using execHelper::core::TargetDescription;
using execHelper::core::CommandCollection;

using execHelper::test::utils::addSettings;
using execHelper::test::OptionsStub;

namespace {
    void setupBasicOptions(OptionsStub& options, const TargetDescription& targets, const CompilerDescription& compilers) {
        addSettings(options.m_settings, "commands", {"build", "clean"});
        addSettings(options.m_settings, "build", {"scons"});
        addSettings(options.m_settings, "clean", {"scons"});
        addSettings(options.m_settings, "scons", {"single-threaded"});

        options.m_targets = targets;
        options.m_compilers = compilers;
    }
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the default options of the scons plugin", "[plugins][scons]") {
        GIVEN("A scons plugin object, basic settings and the multi-threaded option") {
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

            Scons plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("random-command", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& compiler : actualCompilers) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();

                        for(const auto& target : actualTargets) {
                            string targetName = target.getTarget();
                            string runTargetName = target.getRunTarget();
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "-j8", targetName + runTargetName}));
                            expectedQueue.push_back(expectedTask);
                        }
                    }

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the all target of the scons plugin", "[plugins][scons]") {
        GIVEN("A scons plugin object, basic settings and the all target") {
            const TargetDescription actualTargets({"all"}, {""});
            const CompilerDescription actualCompilers(CompilerDescription::CompilerNames({"compiler1", "compiler2"}), 
                                                                                         {"mode1", "mode2"}, 
                                                                                         {"architectureA", "architectureB"},
                                                                                         {"distribution1", "distribution2"}
                                                                                        );

            OptionsStub options;
            setupBasicOptions(options, actualTargets, actualCompilers);
            ExecutorStub executor;
            options.setExecutor(&executor);
            addSettings(options.m_settings["scons"], "clean", "command-line");
            addSettings(options.m_settings["scons"]["clean"], "command-line", {"clean"});

            Scons plugin;

            WHEN("We build the plugin") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& compiler : actualCompilers) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();

                        for(const auto& target : actualTargets) {
                            string targetName = target.getTarget();
                            string runTargetName = target.getRunTarget();
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "-j8"}));
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
                    for(const auto& compiler : options.getCompiler()) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();

                        for(const auto& target : options.getTarget()) {
                            string targetName = target.getTarget();
                            string runTargetName = target.getRunTarget();
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "-j8", "clean"}));
                            expectedQueue.push_back(expectedTask);
                        }
                    }

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the single-threaded option of the scons plugin", "[plugins][scons]") {
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
        addSettings(options.m_settings["scons"], "clean", "command-line");
        addSettings(options.m_settings["scons"]["clean"], "command-line", {"clean"});
            
        Scons plugin;

        GIVEN("A scons plugin object, basic settings and the single-threaded option") {
            addSettings(options.m_settings["scons"], "single-threaded", "yes");

            WHEN("We build the plugin wit the single threaded option") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& compiler : actualCompilers) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();

                        for(const auto& target : actualTargets) {
                            string targetName = target.getTarget();
                            string runTargetName = target.getRunTarget();
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", targetName + runTargetName}));
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
                    for(const auto& compiler : options.getCompiler()) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();

                        for(const auto& target : options.getTarget()) {
                            string targetName = target.getTarget();
                            string runTargetName = target.getRunTarget();
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "clean", targetName + runTargetName}));
                            expectedQueue.push_back(expectedTask);
                        }
                    }

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
                    for(const auto& compiler : actualCompilers) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();

                        for(const auto& target : actualTargets) {
                            string targetName = target.getTarget();
                            string runTargetName = target.getRunTarget();
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "-j8", targetName + runTargetName}));
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
                    for(const auto& compiler : options.getCompiler()) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();

                        for(const auto& target : options.getTarget()) {
                            string targetName = target.getTarget();
                            string runTargetName = target.getRunTarget();
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "-j8", "clean", targetName + runTargetName}));
                            expectedQueue.push_back(expectedTask);
                        }
                    }

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the command line option of the scons plugin", "[plugins][scons]") {
        GIVEN("A scons plugin object, basic settings and the multi-threaded option") {
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
            addSettings(options.m_settings["scons"], "clean", "command-line");
            addSettings(options.m_settings["scons"]["clean"], "command-line", {"clean"});

            vector<string> commandLine({"compiler={COMPILER}", "mode={MODE}", "{ARCHITECTURE}", "hello{DISTRIBUTION}world"});
            addSettings(options.m_settings["scons"], "patterns", {"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"});
            addSettings(options.m_settings["scons"], "command-line", commandLine);
            addSettings(options.m_settings["scons"]["clean"], "command-line", commandLine);
        
            Scons plugin;

            WHEN("We build the plugin") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& compiler : actualCompilers) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();
                        string distributionName = compiler.getDistribution().getDistribution();

                        for(const auto& target : actualTargets) {
                            string targetName = target.getTarget();
                            string runTargetName = target.getRunTarget();
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "-j8", "compiler=" + compilerName, "mode=" + modeName, architectureName, "hello" + distributionName + "world", targetName + runTargetName}));
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
                    for(const auto& compiler : options.getCompiler()) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();
                        string distributionName = compiler.getDistribution().getDistribution();

                        for(const auto& target : options.getTarget()) {
                            string targetName = target.getTarget();
                            string runTargetName = target.getRunTarget();
                            Task expectedTask;
                            expectedTask.append(TaskCollection({"scons", "-j8", "clean", "compiler=" + compilerName, "mode=" + modeName, architectureName, "hello" + distributionName + "world", targetName + runTargetName}));
                            expectedQueue.push_back(expectedTask);
                        }
                    }

                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }
} } }
