#include <string>
#include <vector>
#include <iostream>

#include <catch.hpp>

#include "yaml/yaml.h"
#include "config/settingsNode.h"

using std::string;
using std::vector;

using execHelper::config::SettingsNode;

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

    string basename(const string& file) {
		unsigned int found = file.find_last_of("/\\");
		return file.substr(0,found);
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
} } }
