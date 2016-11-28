#include <catch.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

#include "core/execHelperOptions.h"
#include "commander/commander.h"

#include "utils.h"
#include "executorStub.h"

using std::ofstream;
using std::vector;
using std::string;
using std::cout;
using std::endl;

using execHelper::test::MainVariables;
using execHelper::test::appendVectors;
using execHelper::test::convertToConfig;

using execHelper::core::ExecHelperOptions;
using execHelper::core::Task;
using execHelper::core::test::ExecutorStub;

namespace execHelper { namespace commander { namespace test {
    SCENARIO("Basic test the commander", "[commander]") {
        GIVEN("A fully configured commander and a configuration file") {
            string actualCommand("echo 'Hello world'");
            string command1("command1");
            string command2("command2");
            vector<string> commands({command1, command2});

            string filename("test.exec-helper");

            string config;
            config += convertToConfig("commands", commands);
            config += convertToConfig(command1, vector<string>({"command-line-command"}));
            config += convertToConfig(command2, vector<string>({"command-line-command"}));
            config += convertToConfig("command-line-command", actualCommand);

            ofstream fileWriter;
            fileWriter.open(filename);
            fileWriter << config;
            fileWriter.close();

            Task actualTask1;
            actualTask1.append(actualCommand);
            Task actualTask2;
            actualTask2.append(actualCommand);

            vector<string> arguments;
            arguments.emplace_back("UNITTEST");
            appendVectors(arguments, commands);

            ExecutorStub executor;

            ExecHelperOptions options;
            options.setExecutor(&executor);
            options.parseSettingsFile(filename);

            MainVariables mainVariables(arguments);
            REQUIRE(options.parse(mainVariables.argc, mainVariables.argv.get()));

            Commander commander(options);

            WHEN("We apply the configuration and run the commander") {
                REQUIRE(commander.run() == true);

                THEN("We should get the tasks executed") {
                    REQUIRE(executor.getExecutedTasks() == vector<Task>({actualTask1, actualTask2})); 
                }
            }
        }
    }

    SCENARIO("Test what happens when an unknown command is passed on the command line", "[commander]") {
        GIVEN("A fully configured commander and a configuration file and a command line with an invalid command") {
            string command1("command1");
            string command2("command2");
            vector<string> commands({command1});

            string filename("test.exec-helper");

            string config;
            config += convertToConfig("commands", commands);
            config += convertToConfig(command1, vector<string>({"command-line-command"}));

            ofstream fileWriter;
            fileWriter.open(filename);
            fileWriter << config;
            fileWriter.close();

            vector<string> arguments;
            arguments.emplace_back("UNITTEST");
            appendVectors(arguments, {command2});

            ExecutorStub executor;

            ExecHelperOptions options;
            options.setExecutor(&executor);
            options.parseSettingsFile(filename);

            MainVariables mainVariables(arguments);
            REQUIRE(options.parse(mainVariables.argc, mainVariables.argv.get()));

            Commander commander(options);

            WHEN("We apply the configuration and run the commander") {
                REQUIRE(commander.run() == false);

                THEN("We should have no executed tasks") {
                    REQUIRE(executor.getExecutedTasks() == vector<Task>({})); 
                }
            }
        }
    }

    SCENARIO("Test what happens when an unknown plugin is set", "[commander]") {
        GIVEN("A fully configured commander and a configuration file with an unkown plugin") {
            string command1("command1");
            string command2("command2");
            vector<string> commands({command1, command2});

            string filename("test.exec-helper");

            string config;
            config += convertToConfig("commands", commands);
            config += convertToConfig(command1, vector<string>({"unknown-command-1"}));
            config += convertToConfig(command2, vector<string>({"unknown-command-2"}));

            ofstream fileWriter;
            fileWriter.open(filename);
            fileWriter << config;
            fileWriter.close();

            vector<string> arguments;
            arguments.emplace_back("UNITTEST");
            appendVectors(arguments, commands);

            ExecutorStub executor;

            ExecHelperOptions options;
            options.setExecutor(&executor);
            options.parseSettingsFile(filename);

            MainVariables mainVariables(arguments);
            REQUIRE(options.parse(mainVariables.argc, mainVariables.argv.get()));

            Commander commander(options);

            WHEN("We apply the configuration and run the commander") {
                REQUIRE(commander.run() == false);

                THEN("We should have no executed tasks") {
                    REQUIRE(executor.getExecutedTasks() == vector<Task>({})); 
                }
            }
        }
    }
} } }
