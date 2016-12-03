#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>

#include "plugins/scons.h"

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
    SCENARIO("Testing the scons plugin, multithreaded", "[plugins][scons]") {
        GIVEN("A scons plugin object and some options") {
            const CommandCollection actualCommands = {"init", "build", "run"};
            const TargetDescription actualTarget({"target1", "target2"}, {"runTarget1", "runTarget2"});
            const CompilerDescription::CompilerNames actualCompilerNames({"gcc", "clang"});
            const CompilerDescription::ModeNames actualModes({"debug", "release"});
            const CompilerDescription::ArchitectureNames actualArchitectures({"architectureA", "architectureB"});
            const CompilerDescription actualCompilers(actualCompilerNames, actualModes, actualArchitectures);

            vector<string> arguments;
            arguments.emplace_back("UNITTEST");
            appendVectors(arguments, actualCommands);
            arguments.emplace_back("--target");
            appendVectors(arguments, actualTarget.getTargets());
            arguments.emplace_back("--run-target");
            appendVectors(arguments, actualTarget.getRunTargets());
            arguments.emplace_back("--compiler");
            appendVectors(arguments, actualCompilerNames);
            arguments.emplace_back("--mode");
            appendVectors(arguments, actualModes);
            arguments.emplace_back("--architecture");
            appendVectors(arguments, actualArchitectures);

            string configFile;
            configFile += convertToConfig("commands", {"build", "clean", "distclean"});
            configFile += convertToConfig("build", {"scons"});
            configFile += convertToConfig("clean", {"scons"});
            configFile += convertToConfig("distclean", {"scons"});
            configFile += string("scons:\n")
                            + "    patterns:\n"
                            + "        - COMPILER\n"
                            + "        - MODE\n"
                            + "        - ARCHITECTURE\n"
                            + "    build-dir: build/{COMPILER}/{ARCHITECTURE}/{MODE}\n"
                            + "    single-threaded: no\n"
                            + "    command-line:\n"
                            + "        - compiler={COMPILER}\n"
                            + "        - mode={MODE}\n"
                            + "        - arch={ARCHITECTURE}\n";

            string filename = "scons-test.yaml";
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

            Scons plugin;

            WHEN("We build the plugin") {
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
                        expectedTask.append(TaskCollection({"scons", "-j8", "compiler=" + compilerName, "mode=" + modeName, "arch=" + architectureName, targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected task") {
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

                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();
                        Task expectedTask;
                        expectedTask.append(TaskCollection({"scons", "clean", "-j8", "compiler=" + compilerName, "mode=" + modeName, "arch=" + architectureName, targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected task") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the scons plugin single-threaded", "[plugins][scons]") {
        GIVEN("A scons plugin object and some options") {
            const CommandCollection actualCommands = {"init", "build", "run"};
            const TargetDescription actualTarget({"target1", "target2"}, {"runTarget1", "runTarget2"});
            const CompilerDescription::CompilerNames actualCompilerNames({"gcc", "clang"});
            const CompilerDescription::ModeNames actualModes({"debug", "release"});
            const CompilerDescription::ArchitectureNames actualArchitectures({"architectureA", "architectureB"});
            const CompilerDescription actualCompilers(actualCompilerNames, actualModes, actualArchitectures);

            vector<string> arguments;
            arguments.emplace_back("UNITTEST");
            appendVectors(arguments, actualCommands);
            arguments.emplace_back("--target");
            appendVectors(arguments, actualTarget.getTargets());
            arguments.emplace_back("--run-target");
            appendVectors(arguments, actualTarget.getRunTargets());
            arguments.emplace_back("--compiler");
            appendVectors(arguments, actualCompilerNames);
            arguments.emplace_back("--mode");
            appendVectors(arguments, actualModes);
            arguments.emplace_back("--architecture");
            appendVectors(arguments, actualArchitectures);

            string configFile;
            configFile += convertToConfig("commands", {"build", "clean", "distclean"});
            configFile += convertToConfig("build", {"scons"});
            configFile += convertToConfig("clean", {"scons"});
            configFile += convertToConfig("distclean", {"scons"});
            configFile += string("scons:\n")
                            + "    patterns:\n"
                            + "        - COMPILER\n"
                            + "        - MODE\n"
                            + "        - ARCHITECTURE\n"
                            + "    build-dir: build/{COMPILER}/{ARCHITECTURE}/{MODE}\n"
                            + "    single-threaded: yes\n"
                            + "    command-line:\n"
                            + "        - compiler={COMPILER}\n"
                            + "        - mode={MODE}\n"
                            + "        - arch={ARCHITECTURE}\n";

            string filename = "scons-test.yaml";
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

            Scons plugin;

            WHEN("We build the plugin") {
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
                        expectedTask.append(TaskCollection({"scons", "compiler=" + compilerName, "mode=" + modeName, "arch=" + architectureName, targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected task") {
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

                    for(const auto& target : options.getTarget()) {
                        string targetName = target.getTarget();
                        string runTargetName = target.getRunTarget();
                        Task expectedTask;
                        expectedTask.append(TaskCollection({"scons", "clean", "compiler=" + compilerName, "mode=" + modeName, "arch=" + architectureName, targetName + runTargetName}));
                        expectedQueue.push_back(expectedTask);
                    }
                }

                THEN("We should get the expected task") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing invalid scons plugin commands", "[plugins][scons]") {
        GIVEN("Nothing in particular") {
            ExecHelperOptions options;

            WHEN("We pass an invalid command to scons") {
                Scons scons;
                Task task;

                THEN("It should fail") {
                    scons.apply("distclean", task, options);
                }
            }
        }
    }
} } }
