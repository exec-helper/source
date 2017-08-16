#include <string>
#include <tuple>
#include <vector>

#include <boost/filesystem.hpp>
#include <catch.hpp>

#include "commander/commander.h"
#include "plugins/memory.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::vector;
using std::string;

using boost::filesystem::current_path;

using execHelper::config::Path;
using execHelper::core::Command;
using execHelper::core::EnvironmentCollection;
using execHelper::plugins::Memory;
using execHelper::plugins::MemoryHandler;

using execHelper::test::OptionsStub;
using execHelper::test::utils::addSettings;

namespace execHelper { namespace commander { namespace test {
    SCENARIO("Basic test the commander", "[commander]") {
        GIVEN("A fully configured commander and a configuration file") {
            string command1("command1");
            string command2("command2");
            vector<string> commands({command1, command2});

            OptionsStub options;
            addSettings(options.m_settings, "commands", commands);
            addSettings(options.m_settings, command1, {"memory"});
            addSettings(options.m_settings, command2, {"memory"});

            options.m_commands = commands;

            MemoryHandler memory;

            Commander commander(options, current_path());

            WHEN("We apply the configuration and run the commander") {
                REQUIRE(commander.run());

                THEN("We should get the tasks executed") {
                    const Memory::Memories& memories = memory.getExecutions();

                    REQUIRE(memories.size() == commands.size());
                    for(size_t i = 0; i < memories.size(); ++i) {
                        REQUIRE(memories[i].command == commands[i]);
                    }
                }
            }
        }
    }

    SCENARIO("Test what happens when an unknown command is passed on the command line", "[commander]") {
        GIVEN("A fully configured commander and a configuration file and a command line with an invalid command") {
            string command1("command1");
            string command2("command2");
            vector<string> commands({command1, command2});

            OptionsStub options;
            addSettings(options.m_settings, "commands", commands);
            addSettings(options.m_settings, command1, {"memory"});
            addSettings(options.m_settings, command2, {"memory"});

            options.m_commands = {"command3"};

            Commander commander(options, current_path());

            WHEN("We apply the configuration and run the commander") {
                THEN("It should fail") {
                    REQUIRE_FALSE(commander.run());
                }
            }
        }
    }

    SCENARIO("Test setting the environment") {
        GIVEN("Basic options") {
            const string command1("command1");
            vector<string> commands({command1});

            OptionsStub options;
            addSettings(options.m_settings, "commands", commands);
            addSettings(options.m_settings, command1, {"memory"});

            options.m_commands = commands;

            MemoryHandler memory;
            EnvironmentCollection environment;

            WHEN("We define an empty environment and run the commander") {
                ;
            }

            WHEN("We define an environment and run the commander") {
                environment.emplace("VAR1", "value1");
                environment.emplace("VAR2", "value2");
            }

            THEN_WHEN("We run the commander") {
                EnvironmentCollection commanderEnvironment = environment;
                Commander commander(options, current_path(), std::move(commanderEnvironment));
                bool returnCode = commander.run();

                THEN_CHECK("It must succeed") {
                    REQUIRE(returnCode);
                }

                THEN_CHECK("We must find the same environment") {
                    const Memory::Memories& memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].command == command1);
                    REQUIRE(memories[0].task.getEnvironment() == environment);
                }
            }
        }
    }

    SCENARIO("Test the working directory") {
        GIVEN("A basic setup for the commander") {
            string command("command1");
            vector<string> commands({command});

            OptionsStub options;
            addSettings(options.m_settings, "commands", commands);
            addSettings(options.m_settings, command, {"memory"});

            options.m_commands = commands;

            MemoryHandler memory;

            WHEN("We create and run the commander with an existing working directory") {
                Path actualWorkingDirectory("./tmp");

                Commander commander(options, actualWorkingDirectory);
                bool returnCode = commander.run();

                THEN("It should succeed") {
                    REQUIRE(returnCode);
                }

                THEN("We should find the correct working directory") {
                    const Memory::Memories& memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].task.getWorkingDirectory() == actualWorkingDirectory);
                }
            }
        }
    }
} } }
