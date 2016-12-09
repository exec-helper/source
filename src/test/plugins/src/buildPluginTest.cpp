#include <vector>
#include <string>

#include <catch.hpp>

#include "config/settingsNode.h"
#include "core/compilerDescription.h"
#include "core/compiler.h"
#include "core/mode.h"
#include "core/architecture.h"
#include "core/distribution.h"
#include "core/task.h"
#include "plugins/pluginUtils.h"
#include "plugins/buildPlugin.h"
#include "utils/utils.h"

#include "optionsStub.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::CompilerDescriptionElement;
using execHelper::core::Compiler;
using execHelper::core::Mode;
using execHelper::core::Architecture;
using execHelper::core::Distribution;
using execHelper::core::TaskCollection;
using execHelper::test::utils::addSettings;
using execHelper::test::OptionsStub;

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Test the command line option getter of the build plugin", "[plugins][buildPlugin]") {
        GIVEN("A settings configuration that does not contain a command line setting") {
            string command("random-command");
            string key("random-key");
            SettingsNode rootSettings;
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "random-value");

            CompilerDescriptionElement compilerElement(Compiler("compiler"), Mode("mode"), Architecture("architecture"), Distribution("distribution"));

            WHEN("We request the command line settings") {
                TaskCollection actualTaskCollection = BuildPlugin::getCommandLine(command, rootSettings, compilerElement);

                THEN("We should get an empty response") {
                    REQUIRE(actualTaskCollection.empty() == true);
                }
            }
        }
        GIVEN("A settings configuration that does contains an empty command line setting") {
            string command("random-command");
            string key("command-line");
            SettingsNode rootSettings;
            addSettings(rootSettings, command, key);

            CompilerDescriptionElement compilerElement(Compiler("compiler"), Mode("mode"), Architecture("architecture"), Distribution("distribution"));

            WHEN("We request the command line settings") {
                TaskCollection actualTaskCollection = BuildPlugin::getCommandLine(command, rootSettings, compilerElement);

                THEN("We should get an empty response") {
                    REQUIRE(actualTaskCollection.empty() == true);
                }
            }
        }
        GIVEN("A settings configuration that does contains a command line setting") {
            string command("random-command");
            string key("command-line");
            string value("random-command-line");
            SettingsNode rootSettings;
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, value);

            CompilerDescriptionElement compilerElement(Compiler("compiler"), Mode("mode"), Architecture("architecture"), Distribution("distribution"));

            WHEN("We request the command line settings") {
                TaskCollection actualTaskCollection = BuildPlugin::getCommandLine(command, rootSettings, compilerElement);

                THEN("We should get an empty response") {
                    TaskCollection correctTaskCollection({value});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
        }
        GIVEN("A settings configuration with a command that contains a specific command line setting and an other command that does not") {
            string command("random-command");
            string innerCommand1("inner-random-command1");
            string innerCommand2("inner-random-command2");
            string key("command-line");
            string value1("random-command-line1");
            string value2("random-command-line2");
            SettingsNode rootSettings;
            rootSettings.m_key = command;
            addSettings(rootSettings, innerCommand1, key);
            addSettings(rootSettings[innerCommand1], key, value1);
            addSettings(rootSettings, command, innerCommand2);
            addSettings(rootSettings, innerCommand2, "inner-command-command");
            addSettings(rootSettings, key, value2);

            CompilerDescriptionElement compilerElement(Compiler("compiler"), Mode("mode"), Architecture("architecture"), Distribution("distribution"));

            WHEN("We request the command line settings of the outer command") {
                TaskCollection actualTaskCollection = BuildPlugin::getCommandLine(command, rootSettings, compilerElement);

                THEN("We should get the outer response") {
                    TaskCollection correctTaskCollection({value2});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
            WHEN("We request the command line settings of the first inner command") {
                TaskCollection actualTaskCollection = BuildPlugin::getCommandLine(innerCommand1, rootSettings, compilerElement);

                THEN("We should get the inner response") {
                    TaskCollection correctTaskCollection({value1});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
            WHEN("We request the command line settings of the second inner command") {
                TaskCollection actualTaskCollection = BuildPlugin::getCommandLine(innerCommand2, rootSettings, compilerElement);

                THEN("We should get the general one") {
                    TaskCollection correctTaskCollection({value2});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
        }
        GIVEN("A settings configuration with a command that contains a specific command line setting and an other command that does not and a command-line with patterns") {
            string command("random-command");
            string innerCommand1("inner-random-command1");
            string innerCommand2("inner-random-command2");
            string key("command-line");
            vector<string> value1({"build/{COMPILER}", "mode={MODE}", "hello"});
            vector<string> value2({"other-build/{DISTRIBUTION}-2", "architecture={ARCHITECTURE}", "random-command-line2"});

            vector<string> patterns({"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"});

            SettingsNode rootSettings;
            rootSettings.m_key = command;
            addSettings(rootSettings, innerCommand1, key);
            addSettings(rootSettings[innerCommand1], key, value1);
            addSettings(rootSettings, command, innerCommand2);
            addSettings(rootSettings, innerCommand2, "inner-command-command");
            addSettings(rootSettings, key, value2);
            addSettings(rootSettings, "patterns", patterns);

            CompilerDescriptionElement compilerElement(Compiler("compiler"), Mode("mode"), Architecture("architecture"), Distribution("distribution"));

            vector<string> replacedValue1({"build/compiler", "mode=mode", "hello"});
            vector<string> replacedValue2({"other-build/distribution-2", "architecture=architecture", "random-command-line2"});

            WHEN("We request the command line settings of the outer command") {
                TaskCollection actualTaskCollection = BuildPlugin::getCommandLine(command, rootSettings, compilerElement);

                THEN("We should get the outer response") {
                    TaskCollection correctTaskCollection({replacedValue2});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
            WHEN("We request the command line settings of the first inner command") {
                TaskCollection actualTaskCollection = BuildPlugin::getCommandLine(innerCommand1, rootSettings, compilerElement);

                THEN("We should get the inner response") {
                    TaskCollection correctTaskCollection({replacedValue1});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
            WHEN("We request the command line settings of the second inner command") {
                TaskCollection actualTaskCollection = BuildPlugin::getCommandLine(innerCommand2, rootSettings, compilerElement);

                THEN("We should get the general one") {
                    TaskCollection correctTaskCollection({replacedValue2});
                    REQUIRE(actualTaskCollection == correctTaskCollection);
                }
            }
        }
    }

    SCENARIO("Test the build dir option getter", "[plugins][buildPlugin]") {
        GIVEN("A compiler element and a build dir configuration") {
            string command("random-command");
            string key("build-dir");
            SettingsNode rootSettings;
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "build/{COMPILER}/blaat/{MODE}hello/world{ARCHITECTURE}///{TEST}///{DISTRIBUTION}");
            addSettings(rootSettings, getPatternsKey(), {"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"});

            CompilerDescriptionElement compiler(Compiler("compiler1"), Mode("mode1"), Architecture("architecture1"), Distribution("distribution1"));
            TaskCollection correctBuildDir({"build/compiler1/blaat/mode1hello/worldarchitecture1///{TEST}///distribution1"});

            WHEN("We get the build dir") {
                TaskCollection actualBuildDir = BuildPlugin::getBuildDir(command, rootSettings, compiler);

                THEN("They should match") {
                    REQUIRE(actualBuildDir ==  correctBuildDir);
                }
            }
        }
    }

    SCENARIO("Test the multi threaded setting", "[plugins][buildPlugin]") {
        string command("random-command");
        string key("single-threaded");

        SettingsNode rootSettings;
        OptionsStub options;

        GIVEN("A default configuration setting") {
            bool correctResult = true;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                }
            }
        }
        GIVEN("A default configuration setting with an empty value for the multi-threaded setting") {
            addSettings(rootSettings, command, key);
            bool correctResult = true;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                }
            }
        }
        GIVEN("A default configuration setting with the multi-threaded setting set") {
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "no");
            bool correctResult = true;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                }
            }
        }
        GIVEN("A default configuration setting with the single-threaded setting set") {
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "yes");
            bool correctResult = false;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                }
            }
        }
        GIVEN("A default configuration setting with the single-threaded setting set on different levels") {
            string subCommand("sub-command");
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "no");
            addSettings(rootSettings[command], subCommand, key);
            addSettings(rootSettings[command][subCommand], key, "yes");
            bool correctResult = true;
            bool correctSubResult = false;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 
                bool actualSubResult = BuildPlugin::getMultiThreaded(subCommand, rootSettings[command], options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                    REQUIRE(actualSubResult == correctSubResult);
                }
            }
        }
        GIVEN("A default configuration setting with the single-threaded option set") {
            addSettings(rootSettings, command, key);
            addSettings(rootSettings[command], key, "no");
            bool correctResult = false;

            options.m_singleThreaded = true;

            WHEN("We request the multi threaded settings") {
                bool actualResult = BuildPlugin::getMultiThreaded(command, rootSettings, options); 

                THEN("They should match") {
                    REQUIRE(actualResult == correctResult);
                }
            }
        }
    }
} } }
