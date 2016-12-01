#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <catch.hpp>

#include "yaml/yaml.h"
#include "config/settingsNode.h"

#include "utils/utils.h"

using std::string;
using std::vector;
using std::stringstream;
using std::endl;

using execHelper::config::SettingsNode;
using execHelper::test::utils::basename;

namespace {
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

    string convertToConfig(string key, string value) {
        return string(key + YAML_CONFIG_KEY_DELIMITER + value);
    }
}

namespace execHelper { namespace yaml { namespace test {
    SCENARIO("Yaml wrapper test", "[yaml][yamlwrapper]") {
        GIVEN("A yaml config string to parse") {
            const string key("commands");
            const vector<string> values = {"command1", "command2", "command3"};

            const string key2("command2");
            const string values2("command2-option");

            string yamlConfig = convertToConfig(key, values);
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
            vector<string> correctRun = {"shellRunner"};
            vector<string> correctAnalyze = {"cppcheck", "clang-static-analyzer", "pmd", "simian"};
            vector<string> correctSubmodules = {"3rdparty/Catch", "3rdparty/benchmark"};
            vector<string> correctSconsPatterns = {"COMPILER", "MODE"};
            string correctSconsBuildDir("build/{COMPILER}/{MODE}");
            string correctSconsSingleThreaded("yes");
            string correctSconsCommandLine("compiler={COMPILER} mode={MODE}");
            string correctPmdAutoInstall("yes");
            string correctRunCommandLine("echo \"hello\"");

            WHEN("We pass the config to the yaml wrapper") {
                Yaml yaml(file);

                THEN("We should find all combinations in order") {
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
                    REQUIRE(yaml.getValue({"shellRunner", "command-line"}) == correctRunCommandLine);
                }
            }
        }
    }

    SCENARIO("Yaml wrapper tree test", "[yaml][yamlwrapper]") {
        GIVEN("A yaml config string to parse") {
            YamlFile file;
            file.file = basename(__FILE__) + "/../test.yaml";

            vector<string> correctCommands = {"init", "build", "run", "analyze"};
            vector<string> correctInit = {"git-submodules", "configure"};
            vector<string> correctBuild = {"scons", "make"};
            vector<string> correctRun = {"shellRunner"};
            vector<string> correctAnalyze = {"cppcheck", "clang-static-analyzer", "pmd", "simian"};
            vector<string> correctSubmodules = {"3rdparty/Catch", "3rdparty/benchmark"};
            vector<string> correctSconsPatterns = {"COMPILER", "MODE"};
            string correctSconsBuildDir("build/{COMPILER}/{MODE}");
            string correctSconsSingleThreaded("yes");
            string correctSconsCommandLine("compiler={COMPILER} mode={MODE}");
            string correctPmdAutoInstall("yes");
            string correctRunCommandLine("echo \"hello\"");

            WHEN("We pass the config to the yaml wrapper") {
                Yaml yaml(file);

                THEN("We should find them all in the subtree") {
                    SettingsNode settings;
                    yaml.getTree({}, settings);
                    REQUIRE(settings["commands"].toStringCollection() == correctCommands);
                    REQUIRE(settings["init"].toStringCollection() == correctInit);
                    REQUIRE(settings["build"].toStringCollection() == correctBuild);
                    REQUIRE(settings["run"].toStringCollection() == correctRun);
                    REQUIRE(settings["analyze"].toStringCollection() == correctAnalyze);
                    REQUIRE(settings["git-submodules"]["submodules"].toStringCollection() == correctSubmodules);
                    REQUIRE(settings["scons"]["patterns"].toStringCollection() == correctSconsPatterns);

                    REQUIRE(settings["scons"]["build-dir"].toStringCollection()[0] == correctSconsBuildDir);
                    REQUIRE(settings["scons"]["single-threaded"].toStringCollection()[0] == correctSconsSingleThreaded);
                    REQUIRE(settings["scons"]["command-line"].toStringCollection()[0] == correctSconsCommandLine);
                    REQUIRE(settings["pmd"]["auto-install"].toStringCollection()[0] == correctPmdAutoInstall);
                    REQUIRE(settings["shellRunner"]["command-line"].toStringCollection()[0] == correctRunCommandLine);
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
                    REQUIRE(yaml.getValue({"commands"}).empty() == true);
                    REQUIRE(yaml.getValue({"commands", "command1"}).empty() == true);
                }
                THEN("We should not be able to get any value collection") {
                    REQUIRE(yaml.getValueCollection({"commands"}).empty() == true);
                    REQUIRE(yaml.getValueCollection({"commands", "command1"}).empty() == true);
                }
                THEN("We should not be able to get a tree and should give the strong guarantee") {
                    string settingsKey("blaat");
                    SettingsNode settings;
                    settings.m_key = settingsKey;
                    REQUIRE(yaml.getTree({"commands"}, settings) == false);
                    REQUIRE(yaml.getTree({"commands", "command1"}, settings) == false);

                    // Check the strong guarantee
                    REQUIRE(settings.m_key == settingsKey);
                    REQUIRE(settings.m_values.empty() == true);
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
                    REQUIRE(yaml.getValue({"commands"}).empty() == true);
                    REQUIRE(yaml.getValue({"commands", "command1"}).empty() == true);
                }
                THEN("We should not be able to get any value collection") {
                    REQUIRE(yaml.getValueCollection({"commands"}).empty() == true);
                    REQUIRE(yaml.getValueCollection({"commands", "command1"}).empty() == true);
                }
                THEN("We should not be able to get a tree and should give the strong guarantee") {
                    string settingsKey("blaat");
                    SettingsNode settings;
                    settings.m_key = settingsKey;
                    REQUIRE(yaml.getTree({"commands"}, settings) == false);
                    REQUIRE(yaml.getTree({"commands", "command1"}, settings) == false);

                    // Check the strong guarantee
                    REQUIRE(settings.m_key == settingsKey);
                    REQUIRE(settings.m_values.empty() == true);
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
                    REQUIRE(yaml.getValue({"invalid-key"}).empty() == true);
                    REQUIRE(yaml.getValue({"invalid-key", "invalid-subkey"}).empty() == true);
                }
                THEN("We should not be able to get invalid values collections") {
                    REQUIRE(yaml.getValueCollection({"invalid-key"}).empty() == true);
                    REQUIRE(yaml.getValueCollection({"invalid-key", "invalid-subkey"}).empty() == true);
                }
                THEN("We should not be able to get invalid trees and the settings node is unaltered") {
                    string settingsKey("some-key");
                    SettingsNode settings;
                    settings.m_key = settingsKey;

                    REQUIRE(yaml.getTree({"invalid-key"}, settings) == false);
                    REQUIRE(yaml.getTree({"invalid-key", "invalid-subkey"}, settings) == false);

                    REQUIRE(settings.m_key == settingsKey);
                    REQUIRE(settings.m_values.empty() == true);
                }
            }
        }
    }
} } }
