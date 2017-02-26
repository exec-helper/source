#include <vector>
#include <map>
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
using std::map;
using std::unique_ptr;
using std::ofstream;

using execHelper::config::SettingsNode;
using execHelper::core::PatternsHandler;

using execHelper::test::OptionsStub;
using execHelper::test::utils::MainVariables;
using execHelper::test::utils::appendVectors;
using execHelper::test::utils::convertToConfig;
using execHelper::test::utils::writeSettingsFile;
using execHelper::test::utils::addSettings;

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("Test the help function", "[core][execHelperOptions]") {
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

        SCENARIO("Test the copy constructor", "[core][execHelperOptions]") {
            GIVEN("An execHelperOptions object to copy") {
                ExecHelperOptions execHelper;

                WHEN("We copy the object") {
                    ExecHelperOptions copy(execHelper);

                    THEN("They should be equal") {
                        REQUIRE(execHelper == copy);
                        REQUIRE_FALSE(execHelper != copy);
                    }
                }
                
                WHEN("We copy and change the object") {
                    ExecHelperOptions copy(execHelper);
                    ExecutorStub executor;
                    copy.setExecutor(&executor);

                    THEN("They should not be equal") {
                        REQUIRE_FALSE(execHelper == copy);
                        REQUIRE(execHelper != copy);
                    }
                }
            }
        }

        SCENARIO("Test the settings related functions", "[core][execHelperOptions]") {
            GIVEN("An options object") {
                const string settingsFile("test-settings-file.exec-helper");
                const string testValueKey1("key1");
                const string testValueKey2("key2");
                const string testValueKey3("key3");
                const string testValueKey3a("key3a");
                const string actualValue1("value1");
                const vector<string> actualValue2({"value2a", "value2b", "value2c"});
                const string testValue3("value3");

                const vector<string> keys = {testValueKey1, testValueKey2, testValueKey3};

                SettingsNode settings;
                addSettings(settings, testValueKey1, actualValue1);
                addSettings(settings, testValueKey2, actualValue2);
                addSettings(settings, testValueKey3, testValueKey3a);
                addSettings(settings[testValueKey3], testValueKey3a, testValue3);

                writeSettingsFile(settingsFile, settings, {});

                ExecHelperOptions options;
                options.parseSettingsFile(settingsFile);

                const ExecHelperOptions constOptions = options;

                // Explicitly add the default root key, since we can not know it
                settings.m_key = "<root>";

                WHEN("We request the root settings") {
                    const SettingsNode& rootSettings = options.getSettings();

                    THEN("We should get the root settings") {
                        REQUIRE(settings == rootSettings);
                    }
                }
                WHEN("We request existing settings") {
                    THEN("We should get the ground truth settings") {
                        for(const auto& key : keys) {
                            REQUIRE(options.getSettings(key) == settings[key]);
                        }
                    }
                }
                WHEN("We request non-existing settings") {
                    THEN("We should get the ground truth settings") {
                        REQUIRE(options.getSettings("non-existing-key") == settings);
                    }
                }
            }
        }

        SCENARIO("Test options with no arguments", "[core][execHelperOptions]") {
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

        SCENARIO("Test options with specific arguments", "[core][execHelperOptions]") {
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
                writeSettingsFile(settingsFile, settings, patterns);
                
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

        SCENARIO("Test the getter for the settings file", "[core][execHelperOptions]") {
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

        SCENARIO("Test the parsing of the settings file", "[core][execHelperOptions]") {
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
                writeSettingsFile(settingsFile, settings, patterns);

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

        SCENARIO("Test the get/set executor settings", "[core][execHelperOptions]") {
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

        SCENARIO("Test the pattern handler related settings", "[core][execHelperOptions]") {
            GIVEN("Some patterns that are registered in an options object") {
                const string settingsFile("test-settings-file.exec-helper");
                const Pattern pattern1("PATTERN1", {"pattern1A", "pattern1B"}, 'a', "pattern1");
                const Pattern pattern2("PATTERN2", {"pattern2A", "pattern2B"}, 'b', "pattern2");
                const Pattern pattern3("PATTERN3", {"pattern3A", "pattern3B"}, 'c', "pattern3");
                const Pattern pattern4("PATTERN4", {"pattern4A", "pattern4B"}, 'd', "pattern4");

                const vector<Pattern> patterns = {pattern1, pattern2, pattern3, pattern4};

                writeSettingsFile(settingsFile, SettingsNode(), patterns);

                ExecHelperOptions options;
                options.parseSettingsFile(settingsFile);

                WHEN("We get the patternshandler") {
                    const PatternsHandler& handler = options.getPatternsHandler();

                    THEN("It should contain the patterns") {
                        for(const auto& pattern : patterns) {
                            REQUIRE(handler.contains(pattern.getKey()) == true);
                        }
                    }
                    THEN("We should find the patterns") {
                        for(const auto& pattern : patterns) {
                            REQUIRE(handler.getPattern(pattern.getKey()) == pattern);
                        }
                    }
                    THEN("We should not find non existing keys") {
                        REQUIRE(handler.contains("non-existing-pattern-key") == false);
                    }
                }
            }
        }

        SCENARIO("Test the values getter based on patterns", "[core][execHelperOptions]") {
            GIVEN("Patterns and an accompanying configuration and command line") {
                const string settingsFile("test-settings-file.exec-helper");
                const vector<string> actualCommands({"random-command"});

                const Pattern pattern1("PATTERN1", {"pattern1A", "pattern1B"}, 'a', "pattern1");
                const Pattern pattern2("PATTERN2", {"pattern2A", "pattern2B"}, 'b', "pattern2");
                const Pattern pattern3("PATTERN3", {"pattern3A", "pattern3B"}, 'c', "pattern3");
                const Pattern pattern4("PATTERN4", {"pattern4A", "pattern4B"}, 'd', "pattern4");
                const vector<Pattern> patterns = {pattern1, pattern2, pattern3, pattern4};

                const vector<string> pattern1Value = {"pattern1A"};
                const vector<string> pattern2Value = {"pattern2A", "pattern2B"};
                const vector<string> pattern3Value = {""};

                writeSettingsFile(settingsFile, SettingsNode(), patterns);

                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                appendVectors(arguments, actualCommands);
                arguments.emplace_back("--" + pattern1.getLongOption());
                appendVectors(arguments, pattern1Value);
                arguments.emplace_back("--" + pattern2.getLongOption());
                appendVectors(arguments, pattern2Value);
                arguments.emplace_back("--" + pattern3.getLongOption());
                appendVectors(arguments, pattern3Value);
                MainVariables mainVariables(arguments);

                ExecHelperOptions options;
                options.parseSettingsFile(settingsFile);
                options.parse(mainVariables.argc, mainVariables.argv.get());

                WHEN("We get the pattern values") {
                    THEN("We should get the passed or default values respectively") {
                        REQUIRE(options.getValues(pattern1) == pattern1Value);
                        REQUIRE(options.getValues(pattern2) == pattern2Value);
                        REQUIRE(options.getValues(pattern3) == pattern3Value);
                        REQUIRE(options.getValues(pattern4) == pattern4.getDefaultValues());
                    }
                }
                WHEN("We request a non-existing pattern") {
                    Pattern nonExisting("do-not-exist", {}, 'o', "not-exist");

                    THEN("We should get the default options") {
                        REQUIRE(options.getValues(nonExisting) == nonExisting.getDefaultValues());
                    }
                }
                WHEN("We permutate over the patterns") {
                    vector<map<PatternKey, PatternValue>> combinations;      
                    for(const auto& p1 : pattern1Value) {
                        for(const auto& p2 : pattern2Value) {
                            for(const auto& p3 : pattern3Value) {
                                for(const auto& p4 : pattern4.getDefaultValues()) {
                                    map<PatternKey, PatternValue> combination;
                                    combination.insert(make_pair(pattern1.getKey(), p1));
                                    combination.insert(make_pair(pattern2.getKey(), p2));
                                    combination.insert(make_pair(pattern3.getKey(), p3));
                                    combination.insert(make_pair(pattern4.getKey(), p4));
                                    combinations.emplace_back(combination);
                                }
                            }
                        }
                    }
                    THEN("We should find the right sequence of permutations") {
                        size_t index = 0;
                        for(const auto& permutation : options.makePatternPermutator({pattern1.getKey(), pattern2.getKey(), pattern3.getKey(), pattern4.getKey()})) {
                            REQUIRE(permutation == combinations[index]);
                            ++index;
                        }
                        REQUIRE(index == combinations.size());
                    } 
                }
                WHEN("We permutate without passing patterns") {
                    THEN("We should get no iterations") {
                        size_t index = 0;
                        for(const auto& permutation : options.makePatternPermutator({})) {
                            ++index;
                        }
                        REQUIRE(index == 0U);
                    }
                }
            }
        }
    }
} }
