#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <catch.hpp>

#include "config/settingsNode.h"
#include "yaml/yaml.h"

#include "utils/utils.h"

using std::pair;
using std::string;
using std::vector;
using std::stringstream;
using std::endl;

using execHelper::config::SettingsNode;
using execHelper::test::utils::basename;
using execHelper::test::utils::convertToConfig;
using execHelper::test::utils::writeSettingsFile;

namespace execHelper { namespace yaml { namespace test {
    SCENARIO("Yaml wrapper test", "[yaml][yamlwrapper]") {
        GIVEN("A yaml config string to parse") {
            const string key("commands");
            const vector<string> values = {"command1", "command2", "command3"};

            const string key2("command2");
            const string values2("command2-option");

            string yamlConfig;
            yamlConfig += convertToConfig(key, values);
            yamlConfig += convertToConfig(key2, values2);

            WHEN("We pass the config to the yaml wrapper") {
                Yaml yaml(yamlConfig);

                THEN("We should find all combinations in order") {
                    REQUIRE(yaml.getValueCollection({key}) == values);
                    REQUIRE(yaml.getValue({key2}) == values2);
                }
            }
        }
    }

    SCENARIO("Extensive Yaml file wrapper test", "[yaml][yamlwrapper]") {
        GIVEN("A yaml config file to parse and the right result") {
            YamlFile file;
            file.file = basename(__FILE__) + "/../test.yaml";

            vector<string> correctCommands = {"init", "build", "run", "analyze"};
            vector<string> correctInit = {"git-submodules", "configure"};
            vector<string> correctBuild = {"scons", "make"};
            vector<string> correctRun = {"shellRunner", "command-line"};
            vector<string> correctAnalyze = {"cppcheck", "clang-static-analyzer", "pmd", "simian"};
            vector<string> correctSubmodules = {"3rdparty/Catch", "3rdparty/benchmark"};
            vector<string> correctSconsPatterns = {"COMPILER", "MODE"};
            string correctSconsBuildDir("build/{COMPILER}/{MODE}");
            string correctSconsSingleThreaded("yes");
            string correctSconsCommandLine("compiler={COMPILER} mode={MODE}");
            string correctPmdAutoInstall("yes");
            string correctRunCommandLine("command-line");

            SettingsNode correctSettings("YamlTest");
            correctSettings.add({"commands"}, correctCommands);
            correctSettings.add({"init"}, correctInit);
            correctSettings.add({"build"}, correctBuild);
            correctSettings.add({"run"}, correctRun);
            correctSettings.add({"analyze"}, correctAnalyze);
            correctSettings.add({"git-submodules", "submodules"}, correctSubmodules);
            correctSettings.add({"scons", "patterns"}, correctSconsPatterns);
            correctSettings.add({"scons", "build-dir"}, correctSconsBuildDir);
            correctSettings.add({"scons", "single-threaded"}, correctSconsSingleThreaded);
            correctSettings.add({"scons", "command-line"}, correctSconsCommandLine);
            correctSettings.add({"pmd", "auto-install"}, correctPmdAutoInstall);
            correctSettings.add({"command-line", "run"}, correctRunCommandLine);

            writeSettingsFile(file.file, correctSettings, {});

            WHEN("We pass the config to the yaml wrapper") {
                Yaml yaml(file);

                THEN("We should find all values") {
                    REQUIRE(yaml.getValueCollection({"commands"}) == correctCommands);
                    REQUIRE(yaml.getValueCollection({"init"}) == correctInit);
                    REQUIRE(yaml.getValueCollection({"build"}) == correctBuild);
                    REQUIRE(yaml.getValueCollection({"run"}) == correctRun);
                    REQUIRE(yaml.getValueCollection({"analyze"}) == correctAnalyze);
                    REQUIRE(yaml.getValueCollection({"git-submodules", "submodules"}) == correctSubmodules);
                    REQUIRE(yaml.getValueCollection({"scons", "patterns"}) == correctSconsPatterns);
                    REQUIRE(yaml.getValue({"scons", "build-dir"}) == correctSconsBuildDir);
                    REQUIRE(yaml.getValue({"scons", "single-threaded"}) == correctSconsSingleThreaded);
                    REQUIRE(yaml.getValue({"scons", "command-line"}) == correctSconsCommandLine);
                    REQUIRE(yaml.getValue({"pmd", "auto-install"}) == correctPmdAutoInstall);
                    REQUIRE(yaml.getValue({"command-line", "run"}) == correctRunCommandLine);
                }

                THEN("We should find them all in the subtree") {
                    SettingsNode settings(correctSettings.key());
                    yaml.getTree({}, settings);
                    REQUIRE(settings.key() == correctSettings.key());
                    REQUIRE(settings["commands"].values() == correctCommands);
                    REQUIRE(settings["init"].values() == correctInit);
                    REQUIRE(settings["build"].values() == correctBuild);
                    REQUIRE(settings["run"].values() == correctRun);
                    REQUIRE(settings["analyze"].values() == correctAnalyze);
                    REQUIRE(settings["git-submodules"]["submodules"].values() == correctSubmodules);
                    REQUIRE(settings["scons"]["patterns"].values() == correctSconsPatterns);
                    REQUIRE(settings["scons"]["build-dir"].values()[0] == correctSconsBuildDir);
                    REQUIRE(settings["scons"]["single-threaded"].values()[0] == correctSconsSingleThreaded);
                    REQUIRE(settings["scons"]["command-line"].values()[0] == correctSconsCommandLine);
                    REQUIRE(settings["pmd"]["auto-install"].values()[0] == correctPmdAutoInstall);
                    REQUIRE(settings["command-line"]["run"].values()[0] == correctRunCommandLine);
                    REQUIRE(settings == correctSettings);
                }
            }
        }
    }

    SCENARIO("Test the parsing of a two dimensional array using lists", "[yaml][yamlwrapper]") {
        GIVEN("A two dimensional array") {
            const string rootKey("root-key");
            const vector<pair<string, vector<string>>> actualArray = {{"key1", {"value1a", "value1b"}}, {"key2", {"value2a", "value2b"}}, {"key3", {"value3a", "value3b", "value3c"}}, {"key4", {"value4"}}};

            string yamlConfig;

            WHEN("Write it as a list of arrays and parse the tree") {
                yamlConfig += rootKey + ":\n";
                for(const auto& firstDimension : actualArray) {
                    yamlConfig += "    - " + firstDimension.first + ": [";
                    for(const auto& secondDimension : firstDimension.second) {
                        yamlConfig += " " + secondDimension;
                        if(secondDimension != firstDimension.second.back()) {
                            yamlConfig += ",";
                        }
                    }
                    yamlConfig += "]\n";
                }


                Yaml yaml(yamlConfig);
                SettingsNode settings(rootKey);

                bool returnCode = yaml.getTree({rootKey}, settings);

                THEN("It should succeed") {
                    REQUIRE(returnCode);
                }

                THEN("We should get the same values") {
                    for(const auto& firstDimension : actualArray) {
                        REQUIRE(settings.get({firstDimension.first}, {"blaat"}) == firstDimension.second);
                    }
                }
            }

            WHEN("Write it as a list of a list and parse the tree") {
                yamlConfig += rootKey + ":\n";
                for(const auto& firstDimension : actualArray) {
                    yamlConfig += "  - " + firstDimension.first + ":\n";
                    for(const auto& secondDimension : firstDimension.second) {
                        yamlConfig += "    - " + secondDimension + "\n";
                    }
                }

                Yaml yaml(yamlConfig);
                SettingsNode settings(rootKey);

                bool returnCode = yaml.getTree({rootKey}, settings);

                THEN("It should succeed") {
                    REQUIRE(returnCode);
                }

                THEN("We should get the same values") {
                    for(const auto& firstDimension : actualArray) {
                        REQUIRE(settings.get({firstDimension.first}, {"blaat"}) == firstDimension.second);
                    }
                }
            }
        }
    }

    SCENARIO("Yaml does not find the supplied configuration file", "[yaml][yamlwrapper]") {
        GIVEN("A configuration file that does not exist") {
            YamlFile file;
            file.file = "non-existing-file.exec-helper";

            WHEN("We pass it to yaml") {
                Yaml yaml(file);

                THEN("We should not be able to get any value") {
                    REQUIRE(yaml.getValue({"commands"}).empty());
                    REQUIRE(yaml.getValue({"commands", "some-command"}).empty());
                }
                THEN("We should not be able to get any value collection") {
                    REQUIRE(yaml.getValueCollection({"commands"}).empty());
                    REQUIRE(yaml.getValueCollection({"commands", "some-command"}).empty());
                }
                THEN("We should not be able to get a tree and should give the strong exception guarantee") {
                    string settingsKey("blaat");
                    SettingsNode settings(settingsKey);
                    REQUIRE_FALSE(yaml.getTree({"commands"}, settings));
                    REQUIRE_FALSE(yaml.getTree({"commands", "some-command"}, settings));

                    // Check the strong exception guarantee
                    REQUIRE(settings.key() == settingsKey);
                    REQUIRE(settings.values().empty());
                }
            }
        }
    }

    SCENARIO("Yaml got passed an invalid configuration", "[yaml][yamlwrapper]") {
        GIVEN("Nothing in particular") {
            WHEN("We forget a delimiter") {
                stringstream configStream;
                configStream    << "commands" << endl
                                << "    - commands1" << endl
                                << "    - commands2" << endl;

                Yaml yaml(configStream.str());
                THEN("We should not be able to get any value") {
                    REQUIRE(yaml.getValue({"commands"}).empty());
                    REQUIRE(yaml.getValue({"commands", "command1"}).empty());
                }
                THEN("We should not be able to get any value collection") {
                    REQUIRE(yaml.getValueCollection({"commands"}).empty());
                    REQUIRE(yaml.getValueCollection({"commands", "command1"}).empty());
                }
                THEN("We should not be able to get a tree and should give the strong exception guarantee") {
                    string settingsKey("blaat");
                    SettingsNode settings(settingsKey);
                    REQUIRE_FALSE(yaml.getTree({"commands"}, settings));
                    REQUIRE_FALSE(yaml.getTree({"commands", "command1"}, settings));

                    // Check the strong exception guarantee
                    REQUIRE(settings.key() == settingsKey);
                    REQUIRE(settings.values().empty());
                }
            }
        }
    }

    SCENARIO("Requesting invalid values", "[yaml][yamlwrapper]") {
        GIVEN("A configuration file") {
            const string key("commands");
            const vector<string> values = {"command1", "command2", "command3"};

            const string key2("command2");
            const string values2("command2-option");

            string yamlConfig = convertToConfig(key, values);
            yamlConfig += convertToConfig(key2, values2);

            WHEN("We create the yaml file") {
                Yaml yaml(yamlConfig);

                THEN("We should not be able to get invalid values") {
                    REQUIRE(yaml.getValue({"invalid-key"}).empty());
                    REQUIRE(yaml.getValue({"invalid-key", "invalid-subkey"}).empty());
                }
                THEN("We should not be able to get invalid values collections") {
                    REQUIRE(yaml.getValueCollection({"invalid-key"}).empty());
                    REQUIRE(yaml.getValueCollection({"invalid-key", "invalid-subkey"}).empty());
                }
                THEN("We should not be able to get invalid trees and the settings node is unaltered") {
                    string settingsKey("some-key");
                    SettingsNode settings(settingsKey);

                    REQUIRE_FALSE(yaml.getTree({"invalid-key"}, settings));
                    REQUIRE_FALSE(yaml.getTree({"invalid-key", "invalid-subkey"}, settings));

                    REQUIRE(settings.key() == settingsKey);
                    REQUIRE(settings.values().empty());
                }
            }
        }
    }
} } }
