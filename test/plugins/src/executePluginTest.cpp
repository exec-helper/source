#include <map>
#include <memory>
#include <vector>

#include <gsl/string_span>

#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "log/assertions.h"
#include "plugins/bootstrap.h"
#include "plugins/clangStaticAnalyzer.h"
#include "plugins/clangTidy.h"
#include "plugins/commandLineCommand.h"
#include "plugins/cppcheck.h"
#include "plugins/executePlugin.h"
#include "plugins/make.h"
#include "plugins/memory.h"
#include "plugins/scons.h"
#include "plugins/selector.h"
#include "plugins/valgrind.h"
#include "unittest/catch.h"

#include "utils/utils.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"

using std::map;
using std::move;
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
using execHelper::plugins::Bootstrap;
using execHelper::plugins::ClangStaticAnalyzer;
using execHelper::plugins::CommandLineCommand;
using execHelper::plugins::Cppcheck;
using execHelper::plugins::ExecutePlugin;
using execHelper::plugins::Make;
using execHelper::plugins::Memory;
using execHelper::plugins::MemoryHandler;
using execHelper::plugins::Scons;
using execHelper::plugins::Selector;
using execHelper::plugins::Valgrind;

using execHelper::test::FleetingOptionsStub;

namespace {
const czstring<> PLUGIN_NAME = "execute-plugin";
const czstring<> MEMORY_KEY = "memory";
const czstring<> PATTERN_KEY = "patterns";

template <typename T> bool checkGetPlugin(const string& pluginName) {
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

    [[nodiscard]] inline const Command& getCommand() const noexcept {
        return m_directCommand;
    }

    [[nodiscard]] inline const Task& getTask() const noexcept { return m_task; }

    [[nodiscard]] inline const VariablesMap& getVariables() const noexcept {
        return m_variablesMap;
    }

    inline void setVariables(const VariablesMap& variables) noexcept {
        m_variablesMap = variables;
    }

    [[nodiscard]] inline const Patterns& getPatterns() const noexcept {
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
SCENARIO("Obtain the plugin name of the execute-plugin", "[execute-plugin]") {
    GIVEN("A plugin") {
        ExecutePlugin plugin({});

        WHEN("We request the plugin name") {
            const string pluginName = plugin.getPluginName();

            THEN("We should find the correct plugin name") {
                REQUIRE(pluginName == PLUGIN_NAME);
            }
        }
    }
}

SCENARIO("Obtaining the default variables map of the execute-plugin",
         "[clang-tidy]") {
    GIVEN("The default fleeting options") {
        FleetingOptionsStub fleetingOptions;
        ExecutePlugin plugin({});

        VariablesMap actualVariables(plugin.getPluginName());

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
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode("test"));
        ExecutePlugin::push(Patterns());

        WHEN("We apply the selector plugin") {
            bool success =
                plugin.apply(task, VariablesMap(PLUGIN_NAME), Patterns());

            THEN("It should succeed") { REQUIRE(success); }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    }
}

SCENARIO("Test the settings node to variables map mapping",
         "[execute-plugin]") {
    MAKE_COMBINATIONS("Of settings node configurations") {
        SettingsNode settings("test-execute-plugin");
        FleetingOptionsStub fleetingOptions;

        MemoryHandler memory;

        CommandCollection commands;
        map<Command, vector<Expected>> expected;

        Patterns configuredPatterns;

        COMBINATIONS("Add a plugin command directly") {
            const Command command = MEMORY_KEY;
            commands.push_back(command);
            settings.add(COMMAND_KEY, command);
            expected.emplace(command, vector<Expected>({Expected(command)}));
        }

        COMBINATIONS("Add a command") {
            const Command command("a-command");
            commands.push_back(command);

            settings.add(COMMAND_KEY, command);
            settings.add(command, MEMORY_KEY);

            expected.emplace(command, vector<Expected>({Expected(command)}));
        }

        COMBINATIONS("Add multiple commands") {
            commands = {"multiple-command1", "multiple-command2"};

            settings.replace(COMMAND_KEY, commands);
            for(const auto& command : commands) {
                expected.emplace(command,
                                 vector<Expected>({Expected(command)}));
                settings.add(command, MEMORY_KEY);
            }
        }

        COMBINATIONS("Add an indirect command") {
            const string command("indirect-command");
            commands.push_back(command);

            const vector<string> directCommands(
                {"direct-command1", "direct-command2"});

            settings.add(COMMAND_KEY, command);
            settings.add(command, directCommands);
            for(const auto& directCommand : directCommands) {
                settings.add(directCommand, MEMORY_KEY);
                expected.emplace(command,
                                 vector<Expected>({Expected(directCommand)}));
            }
        }

        COMBINATIONS("Add some root settings") {
            for(const auto& command : commands) {
                ensures(expected.count(command) > 0U);
                for(auto& expectedTask : expected.at(command)) {
                    const auto& directCommand = expectedTask.getCommand();

                    VariablesMap expectedVariableMap(MEMORY_KEY);
                    expectedVariableMap.add(
                        string(directCommand).append("-root-setting1"),
                        "root-setting-value1");
                    expectedVariableMap.add(
                        string(directCommand).append("-root-setting2"),
                        "root-setting-value2");
                    expectedVariableMap.add(
                        string(directCommand).append("-root-setting3"),
                        "root-setting-value3");

                    for(const auto& key :
                        expectedVariableMap.get<SettingsValues>(
                            SettingsKeys(), SettingsValues())) {
                        settings[MEMORY_KEY][key] = expectedVariableMap[key];
                    }
                    expectedTask.setVariables(expectedVariableMap);
                }
            }
        }

        COMBINATIONS("Add some command specific settings") {
            for(const auto& command : commands) {
                ensures(expected.count(command) > 0U);
                for(auto& expectedTask : expected.at(command)) {
                    const auto& directCommand = expectedTask.getCommand();

                    VariablesMap expectedVariableMap(MEMORY_KEY);
                    expectedVariableMap.add(
                        string(directCommand).append("-specific-setting1"),
                        "specific-setting-value1");
                    expectedVariableMap.add(
                        string(directCommand).append("-specific-setting2"),
                        "specific-setting-value2");
                    expectedVariableMap.add(
                        string(directCommand).append("-specific-setting3"),
                        "specific-setting-value3");

                    for(const auto& key :
                        expectedVariableMap.get<SettingsValues>(
                            SettingsKeys(), SettingsValues())) {
                        settings[MEMORY_KEY][directCommand][key] =
                            expectedVariableMap[key];
                    }
                    expectedTask.setVariables(expectedVariableMap);
                }
            }
        }

        COMBINATIONS("Add generic patterns") {
            if(!commands.empty()) {
                const Pattern pattern1("PATTERN1", {"value1a", "value1b"});
                const Pattern pattern2("PATTERN2", {"value2a", "value2b"});
                const Patterns patterns({pattern1, pattern2});

                for(const auto& pattern : patterns) {
                    configuredPatterns.push_back(pattern);
                    settings.add({MEMORY_KEY, PATTERN_KEY}, pattern.getKey());

                    for(const auto& command : commands) {
                        ensures(expected.count(command) > 0U);
                        for(auto& expectedCommand : expected.at(command)) {
                            expectedCommand.addPattern(pattern);
                        }
                    }
                }
            }
        }

        COMBINATIONS("Add a specific pattern") {
            if(!commands.empty()) {
                const Pattern patternA("PATTERNa", {"valueaa", "valueab"});
                const Pattern patternB("PATTERNb", {"valueba", "valuebb"});
                const Patterns patterns({patternA, patternB});

                const Command& command = commands.front();

                ensures(expected.count(command) > 0U);
                for(auto& expectedCommand : expected.at(command)) {
                    expectedCommand.setPatterns({});
                    for(const auto& pattern : patterns) {
                        configuredPatterns.push_back(pattern);
                        settings.add({MEMORY_KEY, expectedCommand.getCommand(),
                                      PATTERN_KEY},
                                     pattern.getKey());
                        expectedCommand.addPattern(pattern);
                    }
                }
            }
        }

        ExecutePlugin plugin(commands);
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(move(settings));
        ExecutePlugin::push(move(configuredPatterns));

        THEN_WHEN("We apply the execute plugin") {
            Task task;
            bool returnCode =
                plugin.apply(task, VariablesMap("random-thing"), Patterns());

            THEN_CHECK("It should succeed") { REQUIRE(returnCode); }

            THEN_CHECK("It called the right commands") {
                const Memory::Memories& memories = memory.getExecutions();
                auto memory = memories.begin();
                for(const auto& command : commands) {
                    REQUIRE(expected.count(command) > 0U);
                    for(const auto& expectedCommand : expected.at(command)) {
                        REQUIRE(memory != memories.end());
                        REQUIRE(memory->task == expectedCommand.getTask());
                        REQUIRE(memory->patterns ==
                                expectedCommand.getPatterns());
                        for(const auto& key :
                            expectedCommand.getVariables().get<SettingsValues>(
                                SettingsKeys(), SettingsValues())) {
                            REQUIRE(memory->variables.contains(key));
                            REQUIRE(memory->variables[key] ==
                                    expectedCommand.getVariables()[key]);
                        }
                        ++memory;
                    }
                }
            }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    }
}

SCENARIO("Test problematic cases", "[execute-plugin]") {
    GIVEN("A plugin with a non-existing plugin to execute") {
        FleetingOptionsStub fleetingOptions;

        ExecutePlugin plugin({"non-existing-plugin"});
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(SettingsNode("test"));
        ExecutePlugin::push(Patterns());

        WHEN("We execute the plugin") {
            Task task;
            bool returnCode =
                plugin.apply(task, VariablesMap("test"), Patterns());

            THEN("It should not succeed") { REQUIRE_FALSE(returnCode); }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    }
    GIVEN("A plugin that fails to execute") {
        const Command command("command");
        FleetingOptionsStub fleetingOptions;
        fleetingOptions.m_commands = {command};

        SettingsNode settings("test");
        settings.add(COMMAND_KEY, command);
        settings.add(command, MEMORY_KEY);

        ExecutePlugin plugin({"memory", "memory"});
        ExecutePlugin::push(
            gsl::not_null<config::FleetingOptionsInterface*>(&fleetingOptions));
        ExecutePlugin::push(move(settings));
        ExecutePlugin::push(Patterns());

        MemoryHandler memory;
        memory.setReturnCode(false);

        WHEN("We execute the plugin") {
            Task task;
            bool returnCode =
                plugin.apply(task, VariablesMap("test"), Patterns());

            THEN("It should not succeed") { REQUIRE_FALSE(returnCode); }
            THEN("It should have stopped executing after the failure") {
                REQUIRE(memory.getExecutions().size() == 1U);
            }
        }

        ExecutePlugin::popFleetingOptions();
        ExecutePlugin::popSettingsNode();
        ExecutePlugin::popPatterns();
    }
}

// Note: this test requires RTTI support
SCENARIO("Testing the plugin getter", "[execute-plugin]") {
    GIVEN("Nothing in particular") {
        WHEN("We request the respective plugin object") {
            THEN("We should get the appropriate ones") {
                REQUIRE(
                    checkGetPlugin<CommandLineCommand>("command-line-command"));
                REQUIRE(checkGetPlugin<Scons>("scons"));
                REQUIRE(checkGetPlugin<Make>("make"));
                REQUIRE(checkGetPlugin<Bootstrap>("bootstrap"));
                REQUIRE(checkGetPlugin<Cppcheck>("cppcheck"));
                REQUIRE(checkGetPlugin<ClangStaticAnalyzer>(
                    "clang-static-analyzer"));
                REQUIRE(checkGetPlugin<ClangTidy>("clang-tidy"));
                REQUIRE(checkGetPlugin<Selector>("selector"));
                REQUIRE(checkGetPlugin<Memory>("memory"));
                REQUIRE(checkGetPlugin<Valgrind>("valgrind"));
            }
        }
        WHEN("We try to get a non-existing plugin") {
            THEN("We should not get anything") {
                REQUIRE_FALSE(ExecutePlugin::getPlugin("non-existing-plugin"));
            }
        }
    }
}
} // namespace execHelper::plugins::test
