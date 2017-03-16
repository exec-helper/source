#include <string>
#include <tuple>
#include <vector>

#include <catch.hpp>

#include "plugins/memory.h"
#include "commander/commander.h"

#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::vector;
using std::string;

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

            Commander commander(options);

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

            Commander commander(options);

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

            WHEN("We define an empty environment and run the commander") {
                const EnvironmentCollection environment;
                EnvironmentCollection commanderEnvironment = environment;
                Commander commander(options, std::move(commanderEnvironment));
                bool returnCode = commander.run();

                THEN("It must succeed") {
                    REQUIRE(returnCode);
                }

                THEN("We must find the same environment") {
                    const Memory::Memories& memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].command == command1);
                    REQUIRE(memories[0].task.getEnvironment() == environment);
                }
            }

            WHEN("We define an environment and run the commander") {
                const EnvironmentCollection environment = {{"VAR1", "value1"}, {"VAR2", "value2"}};
                EnvironmentCollection commanderEnvironment = environment;
                Commander commander(options, std::move(commanderEnvironment));
                bool returnCode = commander.run();

                THEN("It must succeed") {
                    REQUIRE(returnCode);
                }

                THEN("We must find the same environment") {
                    const Memory::Memories& memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].command == command1);
                    REQUIRE(memories[0].task.getEnvironment() == environment);
                }
            }
        }
    }
} } }
