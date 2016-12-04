#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>

#include "plugins/make.h"

#include "executorStub.h"
#include "core/execHelperOptions.h"
#include "core/compilerDescription.h"
#include "core/targetDescription.h"

#include "utils/utils.h"

using std::vector;
using std::string;
using std::ofstream;

using execHelper::core::test::ExecutorStub;
using execHelper::core::ExecHelperOptions;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CompilerDescription;
using execHelper::core::TargetDescription;
using execHelper::core::CommandCollection;

using execHelper::test::utils::MainVariables;
using execHelper::test::utils::basename;
using execHelper::test::utils::convertToConfig;
using execHelper::test::utils::appendVectors;

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the make plugin, multithreaded", "[plugins][make]") {
        GIVEN("A scons plugin object and some options") {
            const CommandCollection actualCommands({"init", "build", "run"});
            const TargetDescription::TargetCollection actualTargets({"target1", "target2"});
            const TargetDescription::RunTargetCollection actualRunTargets({"run-target1", "run-target2"});
            const CompilerDescription::CompilerNames actualCompilerNames({"gcc", "clang"});
            const CompilerDescription::ModeNames actualModes({"debug", "release"});
            const CompilerDescription::ArchitectureNames actualArchitectures({"i386", "armel"});
            const CompilerDescription actualCompilers(actualCompilerNames, actualModes, actualArchitectures);

            vector<string> arguments;
            arguments.emplace_back("UNITTEST");
            appendVectors(arguments, actualCommands);
            arguments.emplace_back("--compiler");
            appendVectors(arguments, actualCompilerNames);
            arguments.emplace_back("--mode");
            appendVectors(arguments, actualModes);
            arguments.emplace_back("--architecture");
            appendVectors(arguments, actualArchitectures);
            arguments.emplace_back("--target");
            appendVectors(arguments, actualTargets);
            arguments.emplace_back("--run-target");
            appendVectors(arguments, actualRunTargets);

            string configFile;
            configFile += convertToConfig("commands", {"init"});
            configFile += convertToConfig("init", {"bootstrap"});
            configFile += string("make:\n")
                            + "    patterns:\n"
                            + "        - COMPILER\n"
                            + "        - MODE\n"
                            + "        - ARCHITECTURE\n"
                            + "    build-dir: build/{COMPILER}/{MODE}/{ARCHITECTURE}\n"
                            + "    single-threaded: no\n"
                            + "    command-line:\n"
                            + "        - V=1\n"
                            + "        - --dry-run\n";


            string filename = "test-make.exec-helper";
            ofstream fileStream;
            fileStream.open(filename);
            fileStream << configFile;
            fileStream.close();

            ExecutorStub executor;
            MainVariables mainVariables(arguments);
            ExecHelperOptions options; 
            options.setExecutor(&executor);
            options.parseSettingsFile(filename);
            options.parse(mainVariables.argc, mainVariables.argv.get());

            Make plugin;

            WHEN("We use the plugin to build") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    string compilerName = compiler.getCompiler().getName();
                    string modeName = compiler.getMode().getMode();
                    string architectureName = compiler.getArchitecture().getArchitecture();

                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "-j8", "V=1", "--dry-run", "-C", "build/" + compilerName + "/" + modeName + "/" + architectureName + "/" + targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }

            WHEN("We use the plugin to clean") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    string compilerName = compiler.getCompiler().getName();
                    string modeName = compiler.getMode().getMode();
                    string architectureName = compiler.getArchitecture().getArchitecture();

                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "clean", "-j8", "V=1", "--dry-run", "-C", "build/" + compilerName + "/" + modeName + "/" + architectureName + "/" + targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }

        }
    }

    SCENARIO("Testing the make plugin, single-threaded", "[plugins][make]") {
        GIVEN("A scons plugin object and some options") {
            const CommandCollection actualCommands({"init", "build", "run"});
            const TargetDescription::TargetCollection actualTargets({"target1", "target2"});
            const TargetDescription::RunTargetCollection actualRunTargets({"run-target1", "run-target2"});
            const CompilerDescription::CompilerNames actualCompilerNames({"gcc", "clang"});
            const CompilerDescription::ModeNames actualModes({"debug", "release"});
            const CompilerDescription::ArchitectureNames actualArchitectures({"i386", "armel"});
            const CompilerDescription actualCompilers(actualCompilerNames, actualModes, actualArchitectures);

            vector<string> arguments;
            arguments.emplace_back("UNITTEST");
            appendVectors(arguments, actualCommands);
            arguments.emplace_back("--compiler");
            appendVectors(arguments, actualCompilerNames);
            arguments.emplace_back("--mode");
            appendVectors(arguments, actualModes);
            arguments.emplace_back("--architecture");
            appendVectors(arguments, actualArchitectures);
            arguments.emplace_back("--target");
            appendVectors(arguments, actualTargets);
            arguments.emplace_back("--run-target");
            appendVectors(arguments, actualRunTargets);

            string configFile;
            configFile += convertToConfig("commands", {"init"});
            configFile += convertToConfig("init", {"bootstrap"});
            configFile += string("make:\n")
                            + "    patterns:\n"
                            + "        - COMPILER\n"
                            + "        - MODE\n"
                            + "        - ARCHITECTURE\n"
                            + "    single-threaded: yes\n"
                            + "    command-line:\n"
                            + "        - V=1\n"
                            + "        - --dry-run\n";

            string filename = "test-make.exec-helper";
            ofstream fileStream;
            fileStream.open(filename);
            fileStream << configFile;
            fileStream.close();

            ExecutorStub executor;
            MainVariables mainVariables(arguments);
            ExecHelperOptions options; 
            options.setExecutor(&executor);
            options.parseSettingsFile(filename);
            options.parse(mainVariables.argc, mainVariables.argv.get());

            Make plugin;

            WHEN("We use the plugin to build") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "V=1", "--dry-run", "-C", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }



            WHEN("We use the plugin to clean") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"make", "clean", "V=1", "--dry-run", "-C", targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the all target of the make plugin", "[plugins][make]") {
        GIVEN("A scons plugin object and some options") {
            const CommandCollection actualCommands({"init", "build", "run"});
            const CompilerDescription::CompilerNames actualCompilerNames({"gcc", "clang"});
            const CompilerDescription::ModeNames actualModes({"debug", "release"});
            const CompilerDescription::ArchitectureNames actualArchitectures({"i386", "armel"});
            const CompilerDescription actualCompilers(actualCompilerNames, actualModes, actualArchitectures);

            vector<string> arguments;
            arguments.emplace_back("UNITTEST");
            appendVectors(arguments, actualCommands);
            arguments.emplace_back("--compiler");
            appendVectors(arguments, actualCompilerNames);
            arguments.emplace_back("--mode");
            appendVectors(arguments, actualModes);
            arguments.emplace_back("--architecture");
            appendVectors(arguments, actualArchitectures);

            string configFile;
            configFile += convertToConfig("commands", {"init"});
            configFile += convertToConfig("init", {"bootstrap"});
            configFile += string("make:\n")
                            + "    patterns:\n"
                            + "        - COMPILER\n"
                            + "        - MODE\n"
                            + "        - ARCHITECTURE\n"
                            + "    build-dir: build/{COMPILER}/{MODE}/{ARCHITECTURE}\n"
                            + "    single-threaded: no\n"
                            + "    command-line:\n"
                            + "        - V=1\n"
                            + "        - --dry-run\n";


            string filename = "test-make.exec-helper";
            ofstream fileStream;
            fileStream.open(filename);
            fileStream << configFile;
            fileStream.close();

            ExecutorStub executor;
            MainVariables mainVariables(arguments);
            ExecHelperOptions options; 
            options.setExecutor(&executor);
            options.parseSettingsFile(filename);
            options.parse(mainVariables.argc, mainVariables.argv.get());

            Make plugin;

            WHEN("We use the plugin to build") {
                Task task;
                REQUIRE(plugin.apply("build", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    string compilerName = compiler.getCompiler().getName();
                    string modeName = compiler.getMode().getMode();
                    string architectureName = compiler.getArchitecture().getArchitecture();

                    Task expectedTask;
                    expectedTask.append(TaskCollection({"make", "-j8", "V=1", "--dry-run", "-C", "build/" + compilerName + "/" + modeName + "/" + architectureName}));
                    expectedQueue.push_back(expectedTask);
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }

            WHEN("We use the plugin to clean") {
                Task task;
                REQUIRE(plugin.apply("clean", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    string compilerName = compiler.getCompiler().getName();
                    string modeName = compiler.getMode().getMode();
                    string architectureName = compiler.getArchitecture().getArchitecture();

                    Task expectedTask;
                    expectedTask.append(TaskCollection({"make", "clean", "-j8", "V=1", "--dry-run", "-C", "build/" + compilerName + "/" + modeName + "/" + architectureName}));
                    expectedQueue.push_back(expectedTask);
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }

        }
    }

    SCENARIO("Testing invalid make plugin commands", "[plugins][make]") {
        GIVEN("Nothing in particular") {
            ExecHelperOptions options;

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
