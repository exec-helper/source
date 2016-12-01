#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>

#include "plugins/bootstrap.h"

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
    SCENARIO("Testing the bootstrap plugin, multithreaded", "[plugins][bootstrap]") {
        GIVEN("A scons plugin object and some options") {
            const CommandCollection actualCommands = {"init", "build", "run"};
            const CompilerDescription::CompilerNames actualCompilerNames({"gcc", "clang"});
            const CompilerDescription::ModeNames actualModes({"debug", "release"});
            const CompilerDescription actualCompilers(actualCompilerNames, actualModes);

            string bootstrapFilename("unittest-bootstrap.sh");

            vector<string> arguments;
            arguments.emplace_back("UNITTEST");
            appendVectors(arguments, actualCommands);
            arguments.emplace_back("--compiler");
            appendVectors(arguments, actualCompilerNames);
            arguments.emplace_back("--mode");
            appendVectors(arguments, actualModes);

            string configFile;
            configFile += convertToConfig("commands", {"init"});
            configFile += convertToConfig("init", {"bootstrap"});
            configFile += string("bootstrap:\n")
                            + "    patterns:\n"
                            + "        - COMPILER\n"
                            + "        - MODE\n"
                            + "    build-dir: build/{COMPILER}/{MODE}\n"
                            + "    filename: " + bootstrapFilename + "\n";

            string filename = "bootstrap-test.yaml";
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

            Bootstrap plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("init", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    string compilerName = compiler.getCompilers()[0].getName();
                    string modeName = compiler.getModes()[0].getMode();

                    Task expectedTask;
                    expectedTask.append(TaskCollection({"cd", "build/" + compilerName + "/" + modeName, "&&", "./" + bootstrapFilename}));
                    expectedQueue.push_back(expectedTask);
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
        GIVEN("A scons plugin object and some options") {
            const CommandCollection actualCommands = {"init", "build", "run"};
            const CompilerDescription::CompilerNames actualCompilerNames({"gcc", "clang"});
            const CompilerDescription::ModeNames actualModes({"debug", "release"});
            const CompilerDescription actualCompilers(actualCompilerNames, actualModes);

            vector<string> arguments;
            arguments.emplace_back("UNITTEST");
            appendVectors(arguments, actualCommands);
            arguments.emplace_back("--compiler");
            appendVectors(arguments, actualCompilerNames);
            arguments.emplace_back("--mode");
            appendVectors(arguments, actualModes);

            string configFile;
            configFile += convertToConfig("commands", {"init"});
            configFile += convertToConfig("init", {"bootstrap"});
            configFile += string("bootstrap:\n")
                            + "    patterns:\n"
                            + "        - COMPILER\n"
                            + "        - MODE\n"
                            + "    build-dir: build/{COMPILER}/{MODE}\n";

            string filename = "bootstrap-test.yaml";
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

            Bootstrap plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("init", task, options) == true);

                ExecutorStub::TaskQueue expectedQueue;
                for(const auto& compiler : options.getCompiler()) {
                    string compilerName = compiler.getCompilers()[0].getName();
                    string modeName = compiler.getModes()[0].getMode();

                    Task expectedTask;
                    expectedTask.append(TaskCollection({"cd", "build/" + compilerName + "/" + modeName, "&&", "./bootstrap.sh"}));
                    expectedQueue.push_back(expectedTask);
                }

                THEN("We should get the expected tasks") {
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing invalid bootstrap plugin commands", "[plugins][bootstrap]") {
        GIVEN("Nothing in particular") {
            ExecHelperOptions options;

            WHEN("We pass an invalid command to scons") {
                Bootstrap plugin;
                Task task;

                THEN("It should fail") {
                    plugin.apply("build", task, options);
                }
            }
        }
    }
} } }
