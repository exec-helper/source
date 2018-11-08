/**
 *@file Tests properties that each plugin should have
 */
#include "unittest/catch.h"
#include "unittest/rapidcheck.h"

#include "executorStub.h"
#include "fleetingOptionsStub.h"
#include "pluginsGenerators.h"

#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/memory.h"
#include "plugins/plugin.h"

#include "core/coreGenerators.h"

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::PatternValue;
using execHelper::config::PatternValues;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;
using execHelper::plugins::MemoryHandler;

using execHelper::core::test::ExecutorStub;
using execHelper::test::FleetingOptionsStub;
using execHelper::test::propertyTest;

namespace {
constexpr std::string_view patternKey{"BLAAT"};

class PluginPreparation {
  public:
    explicit PluginPreparation(
        const execHelper::plugins::Plugin& plugin) noexcept
        : m_variablesMap("Prepared-variables") {
        m_options.m_commands.push_back("memory");

        m_patterns.emplace_back(Pattern(std::string(patternKey), {"memory"}));

        m_variablesMap = plugin.getVariablesMap(FleetingOptionsStub());
        m_variablesMap.add("command-line", "blaat");
        m_variablesMap.add("build-command", "memory");
        m_variablesMap.add("run-command", "memory");
        m_variablesMap.add("patterns", std::string(patternKey));

        execHelper::plugins::ExecutePlugin::push(
            gsl::not_null<FleetingOptionsInterface*>(&m_options));
        execHelper::plugins::ExecutePlugin::push(SettingsNode("Test"));
        execHelper::plugins::ExecutePlugin::push(Patterns{m_patterns});
    }

    PluginPreparation(const PluginPreparation& other) = delete;
    PluginPreparation(PluginPreparation&& other) = default;

    PluginPreparation& operator=(const PluginPreparation& other) = delete;
    PluginPreparation& operator=(PluginPreparation&& other) = default;

    ~PluginPreparation() {
        execHelper::plugins::ExecutePlugin::popFleetingOptions();
        execHelper::plugins::ExecutePlugin::popSettingsNode();
        execHelper::plugins::ExecutePlugin::popPatterns();
    }

    [[nodiscard]] inline const VariablesMap& variablesMap() const {
        return m_variablesMap;
    }

    [[nodiscard]] inline const Patterns& patterns() const { return m_patterns; }

  private:
    FleetingOptionsStub m_options;
    VariablesMap m_variablesMap;
    Patterns m_patterns;
};
} // namespace

namespace execHelper::plugins::test {
SCENARIO("Test retrieving the name of the plugin",
         "[plugins][generic-plugin]") {
    propertyTest(
        "Test retrieving the name of the plugin",
        [](const std::unique_ptr<Plugin>& plugin) {
            THEN_WHEN("We request the plugin name") {
                auto name = plugin->getPluginName();

                THEN_CHECK("It should not be empty") { REQUIRE(!name.empty()); }

                THEN_CHECK(
                    "It should be in the list of returned plugin names") {
                    const auto& pluginNames = ExecutePlugin::getPluginNames();
                    REQUIRE(std::find(pluginNames.begin(), pluginNames.end(),
                                      name) != pluginNames.end());
                }
            }
        });
}

SCENARIO("Test the pattern keyword for each plugin") {
    REQUIRE(Plugin::getPatternsKey() == "patterns");
}

SCENARIO("Every call to a plugin must lead to at least one registered task") {
    propertyTest(
        "Every call to a plugin must lead to at least one registered task",
        [](const std::unique_ptr<Plugin>& plugin) {
            uint32_t nbOfRegisteredTasks = 0U;

            registerExecuteCallback(
                [&nbOfRegisteredTasks](const core::Task& /*task*/) {
                    ++nbOfRegisteredTasks;
                });
            MemoryHandler memory;

            THEN_WHEN("We apply the plugin") {
                PluginPreparation preparation(*plugin);
                bool result =
                    plugin->apply(core::Task(), preparation.variablesMap(),
                                  preparation.patterns());

                THEN_CHECK("The call must succeed") { REQUIRE(result); }

                THEN_CHECK(
                    "The executor should have been called at least once") {
                    REQUIRE(nbOfRegisteredTasks +
                                memory.getExecutions().size() >
                            0U);
                }
            }
        });
}

SCENARIO("A plugin must not alter the arguments before a given task") {
    propertyTest(
        "A plugin must not alter the arguments already in a given task",
        [](const std::unique_ptr<Plugin>& plugin, const Task& task) {
            RC_PRE(!task.getTask().empty());
            ExecutorStub executor;
            ExecuteCallback executeCallback = [&executor](const Task& task) {
                executor.execute(task);
            };
            registerExecuteCallback(executeCallback);

            MemoryHandler memory;

            THEN_WHEN("We apply the plugin") {
                PluginPreparation preparation(*plugin);
                bool result = plugin->apply(task, preparation.variablesMap(),
                                            preparation.patterns());

                THEN_CHECK("The call must succeed") { REQUIRE(result); }

                Tasks executedTasks = executor.getExecutedTasks();
                auto memories = memory.getExecutions();
                std::transform(memories.begin(), memories.end(),
                               std::back_inserter(executedTasks),
                               [](const auto& mem) { return mem.task; });

                THEN_CHECK("The arguments before the task must remain") {
                    auto task_remains = std::any_of(
                        executedTasks.begin(), executedTasks.end(),
                        [&task](const auto& executedTask) {
                            // Check that any of the executed tasks starts with the content of task
                            auto actual = executedTask.getTask().begin();
                            return std::all_of(
                                task.getTask().begin(), task.getTask().end(),
                                [&actual](const std::string& expected) {
                                    return (*actual++ == expected);
                                });
                        });
                    REQUIRE(task_remains);
                }
            }
        });
}
} // namespace execHelper::plugins::test
