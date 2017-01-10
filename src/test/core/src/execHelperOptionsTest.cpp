#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>

#include <catch.hpp>

#include "log/log.h"

#include "core/execHelperOptions.h"
#include "core/patternsHandler.h"
#include "executorStub.h"
#include "utils/utils.h"

#include "optionsStub.h"

using std::string;
using std::vector;
using std::unique_ptr;
using std::ofstream;

using execHelper::config::SettingsNode;
using execHelper::core::PatternsHandler;

using execHelper::test::OptionsStub;
using execHelper::test::utils::MainVariables;
using execHelper::test::utils::appendVectors;
using execHelper::test::utils::convertToConfig;
using execHelper::test::utils::addSettings;

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
                const bool default_singleThreaded = false;
                const CommandCollection default_commands = {};

                WHEN("We parse the variables") {
                    MainVariables mainVariables(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("We should get the default variables") {
                        REQUIRE(options.getVerbosity() == default_verbosity);
                        REQUIRE(options.getSingleThreaded() == default_singleThreaded);
                        REQUIRE(options.getCommands() == default_commands);
                    }
                }
            }
        }

        SCENARIO("Test options with specific arguments", "[execHelperOptions]") {
            GIVEN("The command line we want to pass using the default long options") {
                const CommandCollection actualCommands = {"init", "build", "run"};

                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                appendVectors(arguments, actualCommands);
                arguments.emplace_back("--verbose");
                arguments.emplace_back("--single-threaded");

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("It should be parsed properly") {
                        REQUIRE(options.getVerbosity() == true);
                        REQUIRE(options.getSingleThreaded() == true);
                        REQUIRE(options.getCommands() == actualCommands);
                    }
                }
            }

            GIVEN("The command line we want to pass using the extra configured long options") {
                const string settingsFile("test-settings-file.exec-helper");
                const CommandCollection actualCommands = {"init", "build", "run"};

                const string pattern1Key("PATTERN1");
                const vector<string> pattern1Value({"test-pattern1A", "test-pattern1B"});
                const string pattern2Key("PATTERN2");
                const vector<string> pattern2Value({"test-pattern2"});
                const string pattern3Key("PATTERN3");
                const vector<string> pattern3Value({"test-pattern3", "pattern2B"});
                const string pattern4Key("PATTERN4");
                const vector<string> pattern4Value({"pattern4A", "pattern4B"});

                const Pattern pattern1(pattern1Key, {"pattern1A", "pattern1B"}, 'a', "patternA");
                const Pattern pattern2(pattern2Key, {"pattern2A", "pattern2B"}, 'b', "patternB");
                const Pattern pattern3(pattern3Key, {"pattern3A", "pattern3B"}, 'c', "patternC");
                const Pattern pattern4(pattern4Key, {"pattern4A", "pattern4B"}, 'd', "patternD");
                const vector<Pattern> patterns = {pattern1, pattern2, pattern3, pattern4};
                SettingsNode settings;
                
                ofstream file;
                file.open(settingsFile, std::ios::out | std::ios::trunc);
                file << convertToConfig(settings, patterns);
                file.close();

                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                appendVectors(arguments, actualCommands);
                arguments.emplace_back("--verbose");
                arguments.emplace_back("--single-threaded");
                arguments.emplace_back("--patternA");
                appendVectors(arguments, pattern1Value);
                arguments.emplace_back("--patternB");
                appendVectors(arguments, pattern2Value);
                arguments.emplace_back("--patternC");
                appendVectors(arguments, pattern3Value);
                arguments.emplace_back("--patternD");
                appendVectors(arguments, pattern4Value);

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables(arguments);

                    ExecHelperOptions options; 
                    REQUIRE(options.parseSettingsFile(settingsFile));
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("It should be parsed properly") {
                        REQUIRE(options.getVerbosity() == true);
                        REQUIRE(options.getSingleThreaded() == true);
                    }
                    THEN("The patterns should contain the right values") {
                        const PatternsHandler& handler = options.getPatternsHandler();
                        REQUIRE(handler.contains(pattern1Key) == true);
                        REQUIRE(handler.contains(pattern2Key) == true);
                        REQUIRE(handler.contains(pattern3Key) == true);
                        REQUIRE(handler.contains(pattern4Key) == true);
                        REQUIRE(options.getValues(handler.getPattern(pattern1Key)) == pattern1Value);
                        REQUIRE(options.getValues(handler.getPattern(pattern2Key)) == pattern2Value);
                        REQUIRE(options.getValues(handler.getPattern(pattern3Key)) == pattern3Value);
                        REQUIRE(options.getValues(handler.getPattern(pattern4Key)) == pattern4Value);
                    }
                }
            }

            GIVEN("The command line we want to pass using long options") {
                const CommandCollection actualCommands = {"init", "build", "run"};

                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                appendVectors(arguments, actualCommands);
                arguments.emplace_back("-v");
                arguments.emplace_back("-u");

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("It should be parsed accordingly") {
                        REQUIRE(options.getVerbosity() == true);
                        REQUIRE(options.getSingleThreaded() == true);
                        REQUIRE(options.getCommands() == actualCommands);
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
            GIVEN("A file containing specific default settings for patterns") {
                const string settingsFile("test-settings-file.exec-helper");
                const string commandKey("commands");
                const string command1Key("command1");
                const string pattern1Key("PATTERN1");
                const string pattern2Key("PATTERN2");
                const string pattern3Key("PATTERN3");
                const string pattern4Key("PATTERN4");

                const vector<string> commandValues = {command1Key, "command2", "command3"};
                const vector<string> command1Values = {"command1a", "command1b"};

                const Pattern pattern1(pattern1Key, {"pattern1A", "pattern1B"}, 'a', "pattern1");
                const Pattern pattern2(pattern2Key, {"pattern2A", "pattern2B"}, 'b', "pattern2");
                const Pattern pattern3(pattern3Key, {"pattern3A", "pattern3B"}, 'c', "pattern3");
                const Pattern pattern4(pattern4Key, {"pattern4A", "pattern4B"}, 'd', "pattern4");

                const vector<Pattern> patterns = {pattern1, pattern2, pattern3, pattern4};

                SettingsNode settings;
                addSettings(settings, commandKey, commandValues);
                addSettings(settings, command1Key, command1Values);

                ofstream file;
                file.open(settingsFile, std::ios::out | std::ios::trunc);
                file << convertToConfig(settings, patterns);
                file.close();

                ExecHelperOptions options;

                WHEN("We parse the settings file") {
                    REQUIRE(options.parseSettingsFile(settingsFile));

                    const PatternsHandler& handler = options.getPatternsHandler();

                    THEN("We should get the default settings") {
                        for(const auto& pattern : patterns) {
                            REQUIRE(handler.getPattern(pattern.getKey()).getDefaultValues() == pattern.getDefaultValues());
                        }
                    }
                    THEN("We should get the chosen default settings for their respective settings") {
                        REQUIRE(options.getSettings(commandKey).toStringCollection() == commandValues);
                        REQUIRE(options.getSettings(command1Key).toStringCollection() == command1Values);
                        for(const auto& pattern : patterns) {
                            REQUIRE(handler.getPattern(pattern.getKey()) == pattern);
                        }
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
