#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>

#include <catch.hpp>

#include "core/execHelperOptions.h"
#include "executorStub.h"

using std::string;
using std::vector;
using std::unique_ptr;
using std::ofstream;

namespace {
    template<typename T>
    void appendVectors(T& appendTo, const T& appendFrom) {
        appendTo.insert(std::end(appendTo), std::begin(appendFrom), std::end(appendFrom));
    }

    struct MainVariables {
        int argc;
        unique_ptr<char*[]> argv;

        MainVariables(const vector<string>& arguments) {
            argc = static_cast<int>(arguments.size());
            argv.reset(new char*[argc]);

            for(size_t i = 0; i < arguments.size(); ++i) {
                argv.get()[i] = const_cast<char*>(arguments[i].c_str());
            }
        }
    };

    const string YAML_CONFIG_KEY_DELIMITER(": ");
    const string YAML_CONFIG_DELIMITER("\n");
    const string YAML_CONFIG_OPTION_CHARACTER("    - ");

    string convertToConfig(const string& key, const vector<string>& values) {
        string config(key + YAML_CONFIG_KEY_DELIMITER);
        for(const auto& value : values) {
            config += YAML_CONFIG_DELIMITER + YAML_CONFIG_OPTION_CHARACTER + value;
        }
        config += YAML_CONFIG_DELIMITER;
        return config;
    }

    //string convertToConfig(string key, string value) {
        //return string(key + YAML_CONFIG_KEY_DELIMITER + value);
    //}
}

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

                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                appendVectors(arguments, actualCommands);
                arguments.emplace_back("--verbose");
                arguments.emplace_back("--target");
                appendVectors(arguments, actualTarget.getTargets());
                arguments.emplace_back("--run-target");
                appendVectors(arguments, actualTarget.getRunTargets());

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("It should be parsed properly") {
                        REQUIRE(options.getVerbosity() == true);
                        REQUIRE(options.getCommands() == actualCommands);
                        REQUIRE(options.getTarget() == actualTarget);
                    }
                }
            }

            GIVEN("The command line we want to pass using long options") {
                const CommandCollection actualCommands = {"init", "build", "run"};
                const TargetDescription actualTarget({"target1", "target2"}, {"runTarget1", "runTarget2"});

                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                appendVectors(arguments, actualCommands);
                arguments.emplace_back("-v");
                arguments.emplace_back("-t");
                appendVectors(arguments, actualTarget.getTargets());
                arguments.emplace_back("-r");
                appendVectors(arguments, actualTarget.getRunTargets());

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("It should be parsed accordingly") {
                        REQUIRE(options.getVerbosity() == true);
                        REQUIRE(options.getCommands() == actualCommands);
                        REQUIRE(options.getTarget() == actualTarget);
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
