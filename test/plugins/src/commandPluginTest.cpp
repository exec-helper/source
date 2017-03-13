#include <string>

#include "unittest/catch.h"

#include "config/settingsNode.h"
#include "core/task.h"
#include "plugins/commandPlugin.h"
#include "plugins/memory.h"

#include "utils/utils.h"

#include "optionsStub.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

using execHelper::test::OptionsStub;
using execHelper::test::utils::addSettings;

namespace {
    const string PLUGIN_CONFIG_KEY("commands");
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing erroneous configuration conditions for the commandPlugin", "[plugins][commandPlugin]") {
        GIVEN("A basic setup") {
            const string command("some-command");

            OptionsStub options;

            SettingsNode& rootSettings = options.m_settings;

            Task task;
            CommandPlugin plugin;

            WHEN("We apply the plugin") {
                bool return_code = plugin.apply(command, task, options);

                THEN("It should fail") {
                    REQUIRE_FALSE(return_code);
                }
            }
        }
    }

    SCENARIO("Test the commandPlugin plugin", "[plugins][commandPlugin]") {
        GIVEN("A basic setup") {
            const string command("some-command");

            OptionsStub options;

            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, command);
            addSettings(rootSettings, command, "memory");

            MemoryHandler memory;

            Task task;
            CommandPlugin plugin;

            WHEN("We apply the plugin") {
                bool return_code = plugin.apply(command, task, options);

                THEN("The call should succeed") {
                    REQUIRE(return_code);
                }

                THEN("All default actions should be executed") {
                    const Memory::Memories memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].command == command);
                    REQUIRE(memories[0].task == task);
                    REQUIRE(memories[0].options == &options);
                }
            }

            WHEN("We add the command multiple times") {
                addSettings(rootSettings, PLUGIN_CONFIG_KEY, command);
                addSettings(rootSettings, PLUGIN_CONFIG_KEY, command);

                bool return_code = plugin.apply(command, task, options);

                THEN("The call should succeed") {
                    REQUIRE(return_code);
                }

                THEN("All default actions should be executed") {
                    const Memory::Memories memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].command == command);
                    REQUIRE(memories[0].task == task);
                    REQUIRE(memories[0].options == &options);
                }
            }
        }
    }
} } }
