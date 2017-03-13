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

            TaskCollection commandLine({"command1"});
            addSettings(rootSettings[PLUGIN_CONFIG_KEY], "command-line", commandLine);

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, otherCommandKey);

            CommandLineCommand plugin;
            Task task;
            Task expectedTask;

            SettingsNode* settings = &(rootSettings[PLUGIN_CONFIG_KEY]);

            COMBINATIONS("Toggle between general and specific command settings") {
                settings = &rootSettings[PLUGIN_CONFIG_KEY][command];

                commandLine.clear();
                commandLine.emplace_back("command2");
                addSettings(*settings, "command-line", commandLine);
            }

            COMBINATIONS("Set the command line") {
                TaskCollection commandLineValue({"{" + compilerUtil.compiler.getKey() + "}/{" + compilerUtil.mode.getKey() + "}", "{" + targetUtil.target.getKey() + "}/{" + targetUtil.runTarget.getKey() + "}"});
                addSettings(*settings, "command-line", commandLineValue);
                commandLine.insert(commandLine.end(), commandLineValue.begin(), commandLineValue.end());
            }

            expectedTask.append(commandLine);

            ExecutorStub::TaskQueue expectedTasks = getExpectedTasks(expectedTask, compilerUtil, targetUtil);

            bool returnCode = plugin.apply(command, task, options);
            THEN_CHECK("It should succeed") {
                REQUIRE(returnCode);
            }

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
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
