#include <map>
#include <stdexcept>
#include <vector>

#include <gsl/gsl>

#include "config/pattern.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "log/assertions.h"
#include "plugins/executePlugin.h"
#include "unittest/catch.h"

#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"
#include "handlers.h"

using std::map;
using std::move;
using std::runtime_error;
using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::COMMAND_KEY;
using execHelper::config::CommandCollection;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::SettingsValues;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

using execHelper::test::FleetingOptionsStub;

namespace {
const czstring PLUGIN_NAME = "execute-plugin";
const czstring MEMORY_KEY = "memory";

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
using namespace std::literals;

SCENARIO("Testing the default execute settings", "[execute-plugin]") {
    GIVEN("A selector plugin object and the default options") {
        Task task;

        const FleetingOptionsStub options;
        const SettingsNode settings(PLUGIN_NAME);
        const Plugins plugins;
        const PatternsHandler patternsHandler;
        const ExecutionContext context(options, settings, patternsHandler,
                                       plugins);

        WHEN("We apply the selector plugin") {
            auto actualTasks = executeCommands({}, task, context);

            THEN("It should return an empty task list") {
                REQUIRE(actualTasks.empty());
            }
        }
    }
}

SCENARIO("Test the settings node to variables map mapping",
         "[execute-plugin]") {
    const auto plugins = mapToMemories({"memory"s});
    const PatternsHandler patternsHandler;

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

        const ExecutionContext context(fleetingOptions, settings,
                                       patternsHandler, plugins);

        THEN_WHEN("We apply the execute plugin") {
            Task task;
            auto actualTasks = executeCommands(commands, task, context);

            THEN_CHECK("It called the right commands") {
                REQUIRE(actualTasks.size() == commands.size());
            }
        }
    }
}

SCENARIO("Test problematic cases", "[execute-plugin]") {
    GIVEN("A plugin with a non-existing plugin to execute") {
        const FleetingOptionsStub options;
        const SettingsNode settings(PLUGIN_NAME);
        const Plugins plugins;
        const PatternsHandler patternsHandler;
        const ExecutionContext context(options, settings, patternsHandler,
                                       plugins);

        WHEN("We execute the plugin") {
            THEN("It should throw a runtime_error exception") {
                REQUIRE_THROWS_AS(
                    executeCommands({"non-existing-plugin"}, Task(), context),
                    runtime_error);
            }
        }
    }
}
} // namespace execHelper::plugins::test
