#include <string>
#include <vector>
#include <algorithm>

#include "unittest/catch.h"

#include "config/settingsNode.h"
#include "core/task.h"
#include "plugins/commandLineCommand.h"

#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::string;
using std::vector;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::CompilerUtil;
using execHelper::core::test::ExecutorStub;
using execHelper::test::OptionsStub;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::addPatterns;

namespace {
    const string PLUGIN_CONFIG_KEY("command-line-command");
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the configuration settings of the command-line-command plugin", "[plugins][commandLineCommand]") {
        MAKE_COMBINATIONS("Of several settings") {
            const string command("some-command-line-command");

            OptionsStub options;

            TargetUtil targetUtil;
            addPatterns(targetUtil.getPatterns(), options);

            CompilerUtil compilerUtil;
            addPatterns(compilerUtil.getPatterns(), options);

            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, command);
            addSettings(rootSettings[PLUGIN_CONFIG_KEY], "patterns", targetUtil.getKeys());
            addSettings(rootSettings[PLUGIN_CONFIG_KEY], "patterns", compilerUtil.getKeys());

            vector<TaskCollection> commandLines({{"command1", "{" + compilerUtil.compiler.getKey() + "}/{" + compilerUtil.mode.getKey() + "}", "{" + targetUtil.target.getKey() + "}/{" + targetUtil.runTarget.getKey() + "}"}});

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, otherCommandKey);

            CommandLineCommand plugin;
            Task expectedTask;

            SettingsNode* settings = &(rootSettings[PLUGIN_CONFIG_KEY]);

            COMBINATIONS("Toggle between general and specific command settings") {
                settings = &rootSettings[PLUGIN_CONFIG_KEY][command];
            }
            addSettings(*settings, "command-line", commandLines.front());

            COMBINATIONS("Set a multiple command lines") {
                (*settings)["command-line"].m_values.clear();

                addSettings((*settings)["command-line"], "command1", commandLines[0]);
                commandLines.emplace_back(TaskCollection({"command2", "{" + targetUtil.target.getKey() + "}/{" + targetUtil.runTarget.getKey() + "}", "{" + compilerUtil.compiler.getKey() + "}/{" + compilerUtil.mode.getKey() + "}"}));
                addSettings((*settings)["command-line"], "command2", commandLines[1]);
            }

            ExecutorStub::TaskQueue expectedTasks;
            for(const auto& commandLine : commandLines) {
                Task newTask = expectedTask;
                newTask.append(commandLine);
                expectedTasks.emplace_back(newTask);
            }
            ExecutorStub::TaskQueue replacedExpectedTasks = getExpectedTasks(expectedTasks, compilerUtil, targetUtil);

            Task task;
            bool returnCode = plugin.apply(command, task, options);
            THEN_CHECK("It should succeed") {
                REQUIRE(returnCode);
            }

            THEN_CHECK("It called the right commands") {
                REQUIRE(replacedExpectedTasks == options.m_executor.getExecutedTasks());
            }
        }
    }

    SCENARIO("Testing erroneous configuration conditions for the commandLineCommand plugin", "[plugins][commandLineCommand]") {
        GIVEN("A basic setup") {
            const string command("some-command-line-command");

            OptionsStub options;

            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, command);

            Task task;
            CommandLineCommand plugin;

            WHEN("We add no parameter and apply") {
                bool return_code = plugin.apply(command, task, options);

                THEN("The call should not succeed") {
                    REQUIRE_FALSE(return_code);
                }
            }

            WHEN("We add no parameter and apply") {
                addSettings(rootSettings, PLUGIN_CONFIG_KEY, "command-line");
                bool return_code = plugin.apply(command, task, options);

                THEN("The call should not succeed") {
                    REQUIRE_FALSE(return_code);
                }
            }

            WHEN("We add no parameter and apply") {
                addSettings(rootSettings[PLUGIN_CONFIG_KEY], "command-line", "random-value");
                addSettings(rootSettings[PLUGIN_CONFIG_KEY], command, "command-line");
                bool return_code = plugin.apply(command, task, options);

                THEN("The call should not succeed") {
                    REQUIRE_FALSE(return_code);
                }
            }
        }
    }
} } }
