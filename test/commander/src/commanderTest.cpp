#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <gsl/string_span>

#include "commander/commander.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "log/log.h"
#include "plugins/commandPlugin.h"
#include "plugins/memory.h"
#include "plugins/pluginUtils.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::shared_ptr;
using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::CommandCollection;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Path;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::CommandPlugin;
using execHelper::plugins::getPatternsKey;
using execHelper::plugins::Memory;
using execHelper::plugins::MemoryHandler;
using execHelper::plugins::Plugin;
using execHelper::plugins::Plugins;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::utils::getPredefinedPatterns;

namespace filesystem = std::filesystem;

namespace {
const czstring<> COMMANDS_KEY = "commands";
const czstring<> MEMORY_KEY = "memory";
} // namespace

namespace execHelper::commander::test {
SCENARIO("Basic test the commander", "[commander]") {
    MAKE_COMBINATIONS("Of different inputs for the commander") {
        FleetingOptionsStub fleetingOptions;

        ExecutorStub::TaskQueue expectedTasks;

        SettingsNode settings("test");

        Patterns patterns;
        Patterns expectedPatterns = getPredefinedPatterns();
        EnvironmentCollection env;
        Path workingDirectory = filesystem::current_path();

        MemoryHandler memory;
        Commander commander;

        const Command command1("command1");
        fleetingOptions.m_commands.push_back(command1);
        REQUIRE(settings.add(COMMANDS_KEY, command1));
        REQUIRE(settings.add(command1, MEMORY_KEY));

        Task expectedTask;
        expectedTask.setWorkingDirectory(workingDirectory);
        expectedTasks.emplace_back(expectedTask);

        Plugins plugins = {
            {COMMANDS_KEY, shared_ptr<Plugin>(new CommandPlugin())},
            {MEMORY_KEY, shared_ptr<Plugin>(new Memory())}};

        COMBINATIONS("Add multiple commands") {
            const CommandCollection commands(
                {"multiple-command1", "multiple-command2"});
            for(const auto& command : commands) {
                fleetingOptions.m_commands.push_back(command);
                REQUIRE(settings.add(COMMANDS_KEY, command));
                REQUIRE(settings.add(command, MEMORY_KEY));

                Task expectedTask;
                expectedTask.setWorkingDirectory(workingDirectory);
                expectedTasks.emplace_back(expectedTask);
            }
        }

        COMBINATIONS("Add patterns") {
            patterns.emplace_back(Pattern("pattern1", {"value1a", "value1b"}));
            patterns.emplace_back(Pattern("pattern2", {"value2a", "value2b"}));
            patterns.emplace_back(Pattern("pattern3", {"value3a", "value3b"}));
        }

        COMBINATIONS("Change working directory") {
            workingDirectory = "/tmp";

            for(auto& expectedTask : expectedTasks) {
                expectedTask.setWorkingDirectory(workingDirectory);
            }
        }

        COMBINATIONS("Set environment") {
            env.emplace("ENV1", "VALUE1");
            env.emplace("ENV2", "VALUE2");

            for(auto& expectedTask : expectedTasks) {
                expectedTask.setEnvironment(env);
            }
        }

        expectedPatterns.insert(expectedPatterns.end(), patterns.begin(),
                                patterns.end());
        for(const auto& pattern : expectedPatterns) {
            REQUIRE(
                settings.add({MEMORY_KEY, getPatternsKey()}, pattern.getKey()));
        }

        THEN_WHEN("We apply the configuration and run the commander") {
            bool returnCode =
                commander.run(fleetingOptions, settings, patterns,
                              workingDirectory, env, Plugins{plugins});

            THEN_CHECK("It must succeed") { REQUIRE(returnCode); }

            THEN_CHECK("We should get the tasks executed") {
                const Memory::Memories& memories =
                    plugins::MemoryHandler::getExecutions();
                REQUIRE(memories.size() == expectedTasks.size());

                auto expectedTask = expectedTasks.begin();
                for(auto memory = memories.begin(); memory != memories.end();
                    ++expectedTask, ++memory) {
                    REQUIRE(memory->task == *expectedTask);
                    REQUIRE(memory->patterns == expectedPatterns);
                }
            }
        }
    }
}

SCENARIO(
    "Test what happens when an unknown command is passed on the command line",
    "[commander]") {
    GIVEN("A fully configured commander and a configuration file and a command "
          "line with an invalid command") {
        string command1("command1");
        string command2("command2");
        vector<string> commands({command1, command2});

        FleetingOptionsStub fleetingOptions;
        fleetingOptions.m_commands = {"command3"};

        SettingsNode settings("test");
        REQUIRE(settings.add(COMMANDS_KEY, commands));
        REQUIRE(settings.add(command1, MEMORY_KEY));
        REQUIRE(settings.add(command2, MEMORY_KEY));

        Commander commander;

        WHEN("We apply the configuration and run the commander") {
            THEN("It should fail") {
                REQUIRE_FALSE(
                    commander.run(fleetingOptions, settings, Patterns(),
                                  filesystem::current_path(),
                                  EnvironmentCollection(), Plugins()));
            }
        }
    }
}

SCENARIO("Test when no commands are passed", "[commander]") {
    GIVEN("A fully configured commander and no command set") {
        string command1("command1");
        string command2("command2");
        vector<string> commands({command1, command2});

        FleetingOptionsStub fleetingOptions;

        SettingsNode settings("test");
        REQUIRE(settings.add(COMMANDS_KEY, commands));
        REQUIRE(settings.add(command1, MEMORY_KEY));
        REQUIRE(settings.add(command2, MEMORY_KEY));

        Commander commander;

        Plugins plugins = {
            {COMMANDS_KEY, shared_ptr<Plugin>(new CommandPlugin())},
        };

        WHEN("We apply the configuration and run the commander") {
            THEN("It should fail") {
                REQUIRE_FALSE(
                    commander.run(fleetingOptions, settings, Patterns(),
                                  filesystem::current_path(),
                                  EnvironmentCollection(), Plugins{plugins}));
            }
        }
    }
}
} // namespace execHelper::commander::test
