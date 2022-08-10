#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "config/settingsNode.h"
#include "config/yaml.h"

#include "base-utils/configFileWriter.h"
#include "unittest/catch.h"
#include "utils/utils.h"

using std::endl;
using std::pair;
using std::string;
using std::stringstream;
using std::vector;

using execHelper::config::Path;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::SettingsValues;
using execHelper::test::baseUtils::ConfigFileWriter;
using execHelper::test::utils::convertToConfig;
using execHelper::test::utils::writeSettingsFile;

namespace {
const execHelper::config:: // NOLINT(fuchsia-statically-constructed-objects)
    SettingsValue DEFAULT_VALUE("blaat");
const execHelper::config:: // NOLINT(fuchsia-statically-constructed-objects)
    SettingsValues DEFAULT_VALUES({DEFAULT_VALUE});
} // namespace

namespace execHelper::yaml::test {
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
        vector<string> correctCommands = {"init", "build", "run", "analyze"};
        vector<string> correctInit = {"git-submodules", "configure"};
        vector<string> correctBuild = {"scons", "make"};
        vector<string> correctRun = {"shellRunner", "command-line"};
        vector<string> correctAnalyze = {"cppcheck", "clang-static-analyzer",
                                         "pmd", "simian"};
        vector<string> correctSubmodules = {"3rdparty/Catch",
                                            "3rdparty/benchmark"};
        vector<string> correctSconsPatterns = {"COMPILER", "MODE"};
        string correctSconsBuildDir("build/{COMPILER}/{MODE}");
        string correctSconsSingleThreaded("yes");
        string correctSconsCommandLine("compiler={COMPILER} mode={MODE}");
        string correctPmdAutoInstall("yes");
        string correctRunCommandLine("command-line");

        SettingsNode correctSettings("YamlTest");
        REQUIRE(correctSettings.add({"commands"}, correctCommands));
        REQUIRE(correctSettings.add({"init"}, correctInit));
        REQUIRE(correctSettings.add({"build"}, correctBuild));
        REQUIRE(correctSettings.add({"run"}, correctRun));
        REQUIRE(correctSettings.add({"analyze"}, correctAnalyze));
        REQUIRE(correctSettings.add({"git-submodules", "submodules"},
                                    correctSubmodules));
        REQUIRE(
            correctSettings.add({"scons", "patterns"}, correctSconsPatterns));
        REQUIRE(
            correctSettings.add({"scons", "build-dir"}, correctSconsBuildDir));
        REQUIRE(correctSettings.add({"scons", "single-threaded"},
                                    correctSconsSingleThreaded));
        REQUIRE(correctSettings.add({"scons", "command-line"},
                                    correctSconsCommandLine));
        REQUIRE(correctSettings.add({"pmd", "auto-install"},
                                    correctPmdAutoInstall));
        REQUIRE(correctSettings.add({"command-line", "run"},
                                    correctRunCommandLine));

        ConfigFileWriter file;
        writeSettingsFile(file, correctSettings, {});

        WHEN("We pass the config to the yaml wrapper") {
            Yaml yaml(file.getPath());

            THEN("We should find all values") {
                REQUIRE(yaml.getValueCollection({"commands"}) ==
                        correctCommands);
                REQUIRE(yaml.getValueCollection({"init"}) == correctInit);
                REQUIRE(yaml.getValueCollection({"build"}) == correctBuild);
                REQUIRE(yaml.getValueCollection({"run"}) == correctRun);
                REQUIRE(yaml.getValueCollection({"analyze"}) == correctAnalyze);
                REQUIRE(
                    yaml.getValueCollection({"git-submodules", "submodules"}) ==
                    correctSubmodules);
                REQUIRE(yaml.getValueCollection({"scons", "patterns"}) ==
                        correctSconsPatterns);
                REQUIRE(yaml.getValue({"scons", "build-dir"}) ==
                        correctSconsBuildDir);
                REQUIRE(yaml.getValue({"scons", "single-threaded"}) ==
                        correctSconsSingleThreaded);
                REQUIRE(yaml.getValue({"scons", "command-line"}) ==
                        correctSconsCommandLine);
                REQUIRE(yaml.getValue({"pmd", "auto-install"}) ==
                        correctPmdAutoInstall);
                REQUIRE(yaml.getValue({"command-line", "run"}) ==
                        correctRunCommandLine);
            }

            THEN("We should find them all in the subtree") {
                SettingsNode settings(correctSettings.key());
                yaml.getTree({}, &settings);
                REQUIRE(settings.key() == correctSettings.key());
                REQUIRE(settings.get<vector<string>>(
                            "commands", DEFAULT_VALUES) == correctCommands);
                REQUIRE(settings.get<vector<string>>("init", DEFAULT_VALUES) ==
                        correctInit);
                REQUIRE(settings.get<vector<string>>("build", DEFAULT_VALUES) ==
                        correctBuild);
                REQUIRE(settings.get<vector<string>>("run", DEFAULT_VALUES) ==
                        correctRun);
                REQUIRE(settings.get<vector<string>>(
                            "analyze", DEFAULT_VALUES) == correctAnalyze);
                REQUIRE(settings.get<vector<string>>(
                            SettingsKeys({"git-submodules", "submodules"}),
                            DEFAULT_VALUES) == correctSubmodules);
                REQUIRE(settings.get<vector<string>>(
                            SettingsKeys({"scons", "patterns"}),
                            DEFAULT_VALUES) == correctSconsPatterns);
                REQUIRE(settings.get<string>(
                            SettingsKeys({"scons", "build-dir"}),
                            DEFAULT_VALUE) == correctSconsBuildDir);
                REQUIRE(settings.get<string>(
                            SettingsKeys({"scons", "single-threaded"}),
                            DEFAULT_VALUE) == correctSconsSingleThreaded);
                REQUIRE(settings.get<string>(
                            SettingsKeys({"scons", "command-line"}),
                            DEFAULT_VALUE) == correctSconsCommandLine);
                REQUIRE(settings.get<string>(
                            SettingsKeys({"pmd", "auto-install"}),
                            DEFAULT_VALUE) == correctPmdAutoInstall);
                REQUIRE(settings.get<string>(
                            SettingsKeys({"command-line", "run"}),
                            DEFAULT_VALUE) == correctRunCommandLine);
                REQUIRE(settings == correctSettings);
            }
        }
    }
}

SCENARIO("Test the parsing of a two dimensional array using lists",
         "[yaml][yamlwrapper]") {
    GIVEN("A two dimensional array") {
        const string rootKey("root-key");
        const vector<pair<string, vector<string>>> actualArray = {
            {"key1", {"value1a", "value1b"}},
            {"key2", {"value2a", "value2b"}},
            {"key3", {"value3a", "value3b", "value3c"}},
            {"key4", {"value4"}}};

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

            bool returnCode = yaml.getTree({rootKey}, &settings);

            THEN("It should succeed") { REQUIRE(returnCode); }

            THEN("We should get the same values") {
                for(const auto& firstDimension : actualArray) {
                    REQUIRE(settings.get<vector<string>>({firstDimension.first},
                                                         {"blaat"}) ==
                            firstDimension.second);
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

            bool returnCode = yaml.getTree({rootKey}, &settings);

            THEN("It should succeed") { REQUIRE(returnCode); }

            THEN("We should get the same values") {
                for(const auto& firstDimension : actualArray) {
                    REQUIRE(settings.get<vector<string>>({firstDimension.first},
                                                         {"blaat"}) ==
                            firstDimension.second);
                }
            }
        }
    }
}

SCENARIO("Yaml does not find the supplied configuration file",
         "[yaml][yamlwrapper]") {
    GIVEN("A configuration file that does not exist") {
        ConfigFileWriter file;
        const string expectedErrorMessage("bad file");

        WHEN("We pass it to yaml") {
            THEN("We should not get here") {
                REQUIRE_THROWS_AS(Yaml(file.getPath()), YAML::BadFile);
            }
        }
    }
}

SCENARIO("Yaml got passed an invalid configuration", "[yaml][yamlwrapper]") {
    GIVEN("Nothing in particular") {
        WHEN("We forget a delimiter") {
            stringstream configStream;
            configStream << "commands" << endl
                         << "    - commands1" << endl
                         << "    - commands2" << endl;

            Yaml yaml(configStream.str());
            THEN("We should not be able to get any value") {
                REQUIRE(yaml.getValue({"commands"}).empty());
                REQUIRE(yaml.getValue({"commands", "command1"}).empty());
            }
            THEN("We should not be able to get any value collection") {
                REQUIRE(yaml.getValueCollection({"commands"}).empty());
                REQUIRE(
                    yaml.getValueCollection({"commands", "command1"}).empty());
            }
            THEN("We should not be able to get a tree and should give the "
                 "strong exception guarantee") {
                string settingsKey("blaat");
                SettingsNode settings(settingsKey);
                REQUIRE_FALSE(yaml.getTree({"commands"}, &settings));
                REQUIRE_FALSE(
                    yaml.getTree({"commands", "command1"}, &settings));

                // Check the strong exception guarantee
                REQUIRE(settings.key() == settingsKey);
                REQUIRE(settings.get<SettingsValues>(SettingsKeys()) ==
                        std::nullopt);
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
                REQUIRE(
                    yaml.getValue({"invalid-key", "invalid-subkey"}).empty());
            }
            THEN("We should not be able to get invalid values collections") {
                REQUIRE(yaml.getValueCollection({"invalid-key"}).empty());
                REQUIRE(
                    yaml.getValueCollection({"invalid-key", "invalid-subkey"})
                        .empty());
            }
            THEN("We should not be able to get invalid trees and the settings "
                 "node is unaltered") {
                string settingsKey("some-key");
                SettingsNode settings(settingsKey);

                REQUIRE_FALSE(yaml.getTree({"invalid-key"}, &settings));
                REQUIRE_FALSE(
                    yaml.getTree({"invalid-key", "invalid-subkey"}, &settings));

                REQUIRE(settings.key() == settingsKey);
                REQUIRE(settings.get<SettingsValues>(SettingsKeys()) ==
                        std::nullopt);
            }
        }
    }
}
} // namespace execHelper::yaml::test
