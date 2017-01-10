#include <catch.hpp>
#include <vector>
#include <string>
#include <tuple>

#include "plugins/memory.h"
#include "commander/commander.h"

#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::vector;
using std::string;
using std::get;

using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::plugins::Memory;

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

            Commander commander(options);

            WHEN("We apply the configuration and run the commander") {
                REQUIRE(commander.run() == true);

                THEN("We should get the tasks executed") {
                    const Memory::Memories& memories = Memory::getExecutions();

                    REQUIRE(memories.size() == commands.size());
                    for(size_t i = 0; i < memories.size(); ++i) {
                        REQUIRE(memories[i].command == commands[i]);
                    }
                    Memory::reset();
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
                    REQUIRE(commander.run() == false);
                }
            }
        }
    }
} } }
