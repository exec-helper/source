#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include <gsl/string_span>

#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "log/assertions.h"
#include "plugins/commandLineCommand.h"
#include "plugins/executePlugin.h"
#include "plugins/memory.h"
#include "unittest/catch.h"

#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::map;
using std::move;
using std::runtime_error;
using std::shared_ptr;
using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::COMMAND_KEY;
using execHelper::config::CommandCollection;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::SettingsValues;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::CommandLineCommand;
using execHelper::plugins::ExecutePlugin;
using execHelper::plugins::Memory;

using execHelper::test::FleetingOptionsStub;

namespace {
const czstring<> PLUGIN_NAME = "execute-plugin";
const czstring<> MEMORY_KEY = "memory";

template <typename T> auto checkGetPlugin(const string& pluginName) -> bool {
    auto plugin = ExecutePlugin::getPlugin(pluginName);
    auto* derived = dynamic_cast<T*>(
        plugin.get()); // derived will be a nullptr if the cast fails
    return (derived != nullptr);
}

class Expected {
  public:
    explicit Expected(Command directCommand, Task task = Task(),
                      VariablesMap variablesMap = VariablesMap(MEMORY_KEY),
                      Patterns patterns = Patterns())
        : m_directCommand(move(directCommand)),
          m_task(move(task)),
          m_variablesMap(move(variablesMap)),
          m_patterns(move(patterns)) {
        ;
    }

    [[nodiscard]] inline auto getCommand() const noexcept -> const Command& {
        return m_directCommand;
    }

    [[nodiscard]] inline auto getTask() const noexcept -> const Task& {
        return m_task;
    }

    [[nodiscard]] inline auto getVariables() const noexcept
        -> const VariablesMap& {
        return m_variablesMap;
    }

    inline void setVariables(const VariablesMap& variables) noexcept {
        m_variablesMap = variables;
    }

    [[nodiscard]] inline auto getPatterns() const noexcept -> const Patterns& {
        return m_patterns;
    }

    inline void addPattern(const Pattern& pattern) noexcept {
        m_patterns.push_back(pattern);
    }

    inline void setPatterns(const Patterns& patterns) noexcept {
        m_patterns = patterns;
    }

  private:
    Command m_directCommand;
    Task m_task;
    VariablesMap m_variablesMap;
    Patterns m_patterns;
};
} // namespace

namespace execHelper::plugins::test {
SCENARIO("Obtaining the default variables map of the execute-plugin",
         "[clang-tidy]") {
    GIVEN("The default fleeting options") {
        FleetingOptionsStub fleetingOptions;
        ExecutePlugin plugin({});

        VariablesMap actualVariables(PLUGIN_NAME);

        WHEN("We request the variables map") {
            VariablesMap variables = plugin.getVariablesMap(fleetingOptions);

            THEN("We should find the same ones") {
                REQUIRE(variables == actualVariables);
            }
        }
    }
}

SCENARIO("Testing the default execute settings", "[execute-plugin]") {
    GIVEN("A selector plugin object and the default options") {
        ExecutePlugin plugin({});
        Task task;

        FleetingOptionsStub fleetingOptions;
        auto flRaii = ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        auto seRaii = ExecutePlugin::push(SettingsNode("test"));
        auto paRaii = ExecutePlugin::push(Patterns());

        WHEN("We apply the selector plugin") {
            auto actualTasks = plugin.apply(task, VariablesMap(PLUGIN_NAME));

            THEN("It should return an empty task list") {
                REQUIRE(actualTasks.empty());
            }
        }
    }
}

SCENARIO("Test the settings node to variables map mapping",
         "[execute-plugin]") {
    MAKE_COMBINATIONS("Of settings node configurations") {
        SettingsNode settings("test-execute-plugin");
        FleetingOptionsStub fleetingOptions;

        CommandCollection commands;
        map<Command, vector<Expected>> expected;

        Patterns configuredPatterns;

        COMBINATIONS("Add a plugin command directly") {
            const Command command = MEMORY_KEY;
            commands.push_back(command);
            REQUIRE(settings.add(COMMAND_KEY, command));
            expected.emplace(command, vector<Expected>({Expected(command)}));
        }

        COMBINATIONS("Add a command") {
            const Command command("a-command");
            commands.push_back(command);

            REQUIRE(settings.add(COMMAND_KEY, command));
            REQUIRE(settings.add(command, MEMORY_KEY));

            expected.emplace(command, vector<Expected>({Expected(command)}));
        }

        COMBINATIONS("Add multiple commands") {
            commands = {"multiple-command1", "multiple-command2"};

            REQUIRE(settings.replace(COMMAND_KEY, commands));
            for(const auto& command : commands) {
                expected.emplace(command,
                                 vector<Expected>({Expected(command)}));
                REQUIRE(settings.add(command, MEMORY_KEY));
            }
        }

        COMBINATIONS("Add an indirect command") {
            const string command("indirect-command");
            commands.push_back(command);

            const vector<string> directCommands(
                {"direct-command1", "direct-command2"});

            REQUIRE(settings.add(COMMAND_KEY, command));
            REQUIRE(settings.add(command, directCommands));
            for(const auto& directCommand : directCommands) {
                REQUIRE(settings.add(directCommand, MEMORY_KEY));
                expected.emplace(command,
                                 vector<Expected>({Expected(directCommand)}));
            }

            commands = directCommands;
        }

        auto plRaii = ExecutePlugin::push(
            Plugins({{"Memory",
                      shared_ptr<Plugin>(new execHelper::plugins::Memory())}}));
        auto flRaii = ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        auto seRaii = ExecutePlugin::push(move(settings));
        auto paRaii = ExecutePlugin::push(move(configuredPatterns));

        ExecutePlugin plugin(commands);

        THEN_WHEN("We apply the execute plugin") {
            Task task;
            auto actualTasks = plugin.apply(task, VariablesMap("random-thing"));

            THEN_CHECK("It called the right commands") {
                REQUIRE(actualTasks.size() == commands.size());
            }
        }
    }
}

SCENARIO("Test problematic cases", "[execute-plugin]") {
    GIVEN("A plugin with a non-existing plugin to execute") {
        FleetingOptionsStub fleetingOptions;

        auto plRaii = ExecutePlugin::push(
            Plugins({{"Memory",
                      shared_ptr<Plugin>(new execHelper::plugins::Memory())}}));
        auto flRaii = ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        auto seRaii = ExecutePlugin::push(SettingsNode("test"));
        auto paRaii = ExecutePlugin::push(Patterns());

        ExecutePlugin plugin({"non-existing-plugin"});

        WHEN("We execute the plugin") {
            THEN("It should throw a runtime_error exception") {
                REQUIRE_THROWS_AS(plugin.apply(Task(), VariablesMap("test")),
                                  runtime_error);
            }
        }
    }
}

// Note: this test requires RTTI support
SCENARIO("Testing the plugin getter", "[execute-plugin]") {
    GIVEN("Nothing in particular") {
        WHEN("We request the respective plugin object") {
            THEN("We should get the appropriate ones") {
                REQUIRE(checkGetPlugin<const CommandLineCommand>(
                    "command-line-command"));
                REQUIRE(checkGetPlugin<const Memory>("memory"));
            }
        }
        WHEN("We try to get a non-existing plugin") {
            THEN("We should not get anything") {
                REQUIRE_THROWS_AS(
                    ExecutePlugin::getPlugin("non-existing-plugin"),
                    plugins::InvalidPlugin);
            }
        }
    }
}

SCENARIO("Print the execute-plugin summary", "[execute-plugin][success]") {
    ExecutePlugin plugin({});

    WHEN("We request the summary of the plugin") {
        auto summary = plugin.summary();

        THEN("The summary must not be empty") { REQUIRE(!summary.empty()); }
    }
}

SCENARIO("Stream the execute-plugin summary", "[execute-plugin][success]") {
    ExecutePlugin plugin({});

    WHEN("We request the summary of the plugin") {
        std::stringstream summary;
        summary << plugin;

        THEN("The summary must not be empty") {
            REQUIRE(!summary.str().empty());
        }

        THEN("The summary must show the expected message") {
            REQUIRE(summary.str() == plugin.summary());
        }
    }
}
} // namespace execHelper::plugins::test
