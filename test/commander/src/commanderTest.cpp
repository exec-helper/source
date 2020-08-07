#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <gsl/string_span>

#include "commander/commander.h"
#include "config/commandLineOptions.h"
#include "config/environment.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "log/log.h"
#include "plugins/commandPlugin.h"
#include "plugins/memory.h"
#include "plugins/pluginUtils.h"

#include "config/generators.h"
#include "unittest/catch.h"
#include "unittest/rapidcheck.h"
#include "utils/matchers.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::inserter;
using std::make_shared;
using std::shared_ptr;
using std::static_pointer_cast;
using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::CommandCollection;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Path;
using execHelper::config::Patterns;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::CommandPlugin;
using execHelper::plugins::getPatternsKey;
using execHelper::plugins::Plugin;
using execHelper::plugins::Plugins;
using execHelper::plugins::SpecialMemory;

using execHelper::config::test::Unique;
using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::MatchExecution;
using execHelper::test::propertyTest;
using execHelper::test::utils::getPredefinedPatterns;

namespace filesystem = std::filesystem;

namespace {
const czstring<> COMMANDS_KEY = "commands";
const czstring<> MEMORY_KEY = "memory";
} // namespace

namespace execHelper::commander::test {
SCENARIO("Basic test the commander", "[commander][wip]") {
    propertyTest(
        "A commander and properly configured arguments for running it",
        [](SettingsNode settings, const Unique<CommandCollection>& commands,
           const Path& workingDirectory, const Path& rootDirectory,
           const Unique<Patterns>& patterns, const EnvironmentCollection& env) {
            const CommandCollection cmds = commands.get();
            FleetingOptionsStub fleetingOptions;
            fleetingOptions.m_commands = cmds;

            ExecutorStub::TaskQueue expectedTasks(
                commands->size(), Task({}, env, workingDirectory));

            auto memory = make_shared<SpecialMemory>();
            Plugins plugins = {
                {COMMANDS_KEY, shared_ptr<Plugin>(new CommandPlugin())}};
            transform(commands->begin(), commands->end(),
                      inserter(plugins, plugins.end()),
                      [&memory](const auto& key) {
                          return make_pair(move(key),
                                           static_pointer_cast<Plugin>(memory));
                      });

            Patterns expectedPatterns = getPredefinedPatterns(rootDirectory);
            expectedPatterns.insert(expectedPatterns.end(), patterns->begin(),
                                    patterns->end());

            for(const auto& command : cmds) {
                REQUIRE(settings.add(COMMANDS_KEY, command));

                const SettingsKeys keys({command, getPatternsKey()});

                for(const auto& pattern : expectedPatterns) {
                    REQUIRE(settings.add(keys,
                                         pattern.getKey()));
                }
            }

            Commander commander;

            THEN_WHEN("We apply the configuration and run the commander") {
                bool returnCode = commander.run(
                    fleetingOptions, settings, *patterns, workingDirectory, env,
                    Plugins{plugins}, rootDirectory);

                THEN_CHECK("It must succeed") { REQUIRE(returnCode); }

                THEN_CHECK("The expected tasks are executed") {
                    REQUIRE_THAT(
                        memory->getExecutions(),
                        MatchExecution(expectedTasks, expectedPatterns));
                }
            }
        });
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
                REQUIRE_FALSE(commander.run(
                    fleetingOptions, settings, Patterns(),
                    filesystem::current_path(), EnvironmentCollection(),
                    Plugins(), filesystem::current_path()));
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
                REQUIRE_FALSE(commander.run(
                    fleetingOptions, settings, Patterns(),
                    filesystem::current_path(), EnvironmentCollection(),
                    Plugins{plugins}, filesystem::current_path()));
            }
        }
    }
}
} // namespace execHelper::commander::test
