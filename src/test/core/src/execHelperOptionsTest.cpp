#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>

#include <catch.hpp>

#include "log/log.h"
#undef LOG
#define LOG(x)

#undef user_feedback 
#define user_feedback(x)

#include "core/execHelperOptions.h"
#include "executorStub.h"
#include "utils.h"

#include "core/compiler.h"
#include "core/mode.h"
#include "core/compilerDescription.h"

using std::string;
using std::vector;
using std::unique_ptr;
using std::ofstream;

using execHelper::core::CompilerDescription;
using execHelper::core::Compiler;
using execHelper::core::Mode;

using execHelper::test::MainVariables;
using execHelper::test::appendVectors;
using execHelper::test::convertToConfig;

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("Test the help function", "[execHelperOptions]") {
            GIVEN("A command line with the help argument") {
                vector<string> arguments = {"UNITTEST", "--help"};

                WHEN("We convert it to main function arguments") {
                    MainVariables mainVariables(arguments);

                    THEN("The parsing should succeed") {
                        ExecHelperOptions options; 
                        REQUIRE(options.parse(mainVariables.argc, mainVariables.argv.get()));
                        REQUIRE(options.containsHelp() == true);
                        options.printHelp();
                    }
                }
            }
        }

        SCENARIO("Test options with no arguments", "[execHelperOptions]") {
            GIVEN("A command line with no arguments and the default values for each parameter") {
                vector<string> arguments;
                arguments.emplace_back("UNITTEST");

                const bool default_verbosity = false;
                const CommandCollection default_commands = {};
                const TargetDescription::TargetCollection default_targets = {"all"};
                const TargetDescription::RunTargetCollection defaultRunTargets = {"all"};

                TargetDescription defaultTarget(default_targets, defaultRunTargets);

                WHEN("We parse the variables") {
                    MainVariables mainVariables(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("We should get the default variables") {
                        REQUIRE(options.getVerbosity() == default_verbosity);
                        REQUIRE(options.getCommands() == default_commands);
                        REQUIRE(options.getTarget() == defaultTarget);
                    }
                }
            }
        }

        SCENARIO("Test options with specific arguments", "[execHelperOptions]") {
            GIVEN("The command line we want to pass using long options") {
                const CommandCollection actualCommands = {"init", "build", "run"};
                const TargetDescription actualTarget({"target1", "target2"}, {"runTarget1", "runTarget2"});
                const CompilerDescription::CompilerNames actualCompilerNames({"clang", "gcc"});
                const CompilerDescription::ModeNames actualModes({"debug", "release"});
                const CompilerDescription::ArchitectureNames actualArchitectures({"i386", "armel"});
                const CompilerDescription actualCompilers(actualCompilerNames, actualModes, actualArchitectures);

                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                appendVectors(arguments, actualCommands);
                arguments.emplace_back("--verbose");
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

                for(const auto& argument : arguments) {
                    std::cout << argument << " ";
                }
                std::cout << std::endl;

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("It should be parsed properly") {
                        REQUIRE(options.getVerbosity() == true);
                        REQUIRE(options.getCommands() == actualCommands);
                        REQUIRE(options.getTarget() == actualTarget);
                        REQUIRE(options.getCompiler() == actualCompilers);
                    }
                }
            }

            GIVEN("The command line we want to pass using long options") {
                const CommandCollection actualCommands = {"init", "build", "run"};
                const TargetDescription actualTarget({"target1", "target2"}, {"runTarget1", "runTarget2"});
                const CompilerDescription::CompilerNames actualCompilerNames({"clang", "gcc"});
                const CompilerDescription::ModeNames actualModes({"debug", "release"});
                const CompilerDescription::ArchitectureNames actualArchitectures({"i386", "armel"});
                const CompilerDescription actualCompilers(actualCompilerNames, actualModes, actualArchitectures);

                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                appendVectors(arguments, actualCommands);
                arguments.emplace_back("-v");
                arguments.emplace_back("-t");
                appendVectors(arguments, actualTarget.getTargets());
                arguments.emplace_back("-r");
                appendVectors(arguments, actualTarget.getRunTargets());
                arguments.emplace_back("-c");
                appendVectors(arguments, actualCompilerNames);
                arguments.emplace_back("-m");
                appendVectors(arguments, actualModes);
                arguments.emplace_back("-a");
                appendVectors(arguments, actualArchitectures);

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("It should be parsed accordingly") {
                        REQUIRE(options.getVerbosity() == true);
                        REQUIRE(options.getCommands() == actualCommands);
                        REQUIRE(options.getTarget() == actualTarget);
                        REQUIRE(options.getCompiler() == actualCompilers);
                    }
                }
            }

            GIVEN("The command line arguments using the flag for the commands") {
                const CommandCollection actualCommands = {"init", "build", "run"};
                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                arguments.emplace_back("--command");
                appendVectors(arguments, actualCommands);

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("We should get the default variables") {
                        REQUIRE(options.getCommands() == actualCommands);
                    }
                }
            }
        }

        SCENARIO("Test the getter for the settings file", "[execHelperOptions]") {
            GIVEN("A command line without the options for the settings file and an options object") {
                ExecHelperOptions options;
                vector<string> arguments = {"UNITTEST"};

                WHEN("We request the settings file") {
                    MainVariables mainVariables(arguments);

                    THEN("We should get the default") {
                        REQUIRE(options.getSettingsFile(mainVariables.argc, mainVariables.argv.get()) == ".exec-helper");
                    }
                }
            }

            GIVEN("A command line with the long option for the settings file and an options object") {
                string settingsFile("test-settings-file.exec-helper");
                ExecHelperOptions options;
                vector<string> arguments = {"UNITTEST", "--settings-file", settingsFile};

                WHEN("We request the settings file") {
                    MainVariables mainVariables(arguments);

                    THEN("We should get the default") {
                        REQUIRE(options.getSettingsFile(mainVariables.argc, mainVariables.argv.get()) == settingsFile);
                    }
                }
            }

            GIVEN("A command line with the short option for the settings file and an options object") {
                string settingsFile("test-settings-file.exec-helper");
                ExecHelperOptions options;
                vector<string> arguments = {"UNITTEST", "-s", settingsFile};

                WHEN("We request the settings file") {
                    MainVariables mainVariables(arguments);

                    THEN("We should get the default") {
                        REQUIRE(options.getSettingsFile(mainVariables.argc, mainVariables.argv.get()) == settingsFile);
                    }
                }
            }
        }

        SCENARIO("Test the parsing of the settings file", "[execHelperOptions]") {
            GIVEN("A file containing specific settings") {
                const string settingsFile("test-settings-file.exec-helper");
                const string commandsKey("commands");
                const string command1Key("command1");
                const vector<string> commandsValues = {command1Key, "command2", "command3"};
                const vector<string> command1Values = {"command1a", "command1b"};

                ofstream file;
                file.open(settingsFile, std::ios::out | std::ios::trunc);
                file << convertToConfig(commandsKey, commandsValues);
                file << convertToConfig(command1Key, command1Values);
                file.close();

                ExecHelperOptions options;

                WHEN("We parse the settings file") {
                    REQUIRE(options.parseSettingsFile(settingsFile));

                    THEN("We should get the settings found") {
                        REQUIRE(options.getSettings(commandsKey).toStringCollection() == commandsValues);
                        REQUIRE(options.getSettings(command1Key).toStringCollection() == command1Values);
                    }
                }
            }
        }
        SCENARIO("Test the get/set executor settings", "[execHelperOptions]") {
            GIVEN("An executor to set") {
                ExecutorStub executor;
                ExecHelperOptions options;

                WHEN("We set the executor") {
                    options.setExecutor(&executor);

                    THEN("We should get the same executor") {
                        REQUIRE(options.getExecutor() == &executor);
                    }
                }
            }
        }
    }
} }
