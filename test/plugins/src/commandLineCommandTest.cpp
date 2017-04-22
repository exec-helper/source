#include <algorithm>
#include <string>
#include <vector>

#include "config/settingsNode.h"
#include "core/task.h"
#include "plugins/commandLineCommand.h"
#include "unittest/catch.h"
#include "utils/combinationHelpers.h"
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
using execHelper::test::utils::copyAndAppend;
using execHelper::test::utils::combineVectors;
using execHelper::test::utils::addPatterns;

using execHelper::test::combinationHelpers::setEnvironment;

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
            rootSettings.add({PLUGIN_CONFIG_KEY, "patterns"}, targetUtil.getKeys());
            rootSettings.add({PLUGIN_CONFIG_KEY, "patterns"}, compilerUtil.getKeys());

            const string commandKey("command1");
            const TaskCollection command1({"command1"});
            const TaskCollection command2({"{" + compilerUtil.compiler.getKey() + "}/{" + compilerUtil.mode.getKey() + "}", "{" + targetUtil.target.getKey() + "}/{" + targetUtil.runTarget.getKey() + "}"});
           const vector<TaskCollection> commandLines({command1, command2});

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");

            CommandLineCommand plugin;
            Task expectedTask;
            std::vector<TaskCollection> commandLine;

            SettingsNode::SettingsKeys baseSettingsKeys = {PLUGIN_CONFIG_KEY};
            SettingsNode::SettingsKeys otherBaseSettingsKeys = {PLUGIN_CONFIG_KEY, otherCommandKey};

            COMBINATIONS("Toggle between general and specific command settings") {
                baseSettingsKeys.push_back(command);
            }
            rootSettings.add(copyAndAppend(baseSettingsKeys, "command-line"), command1);
            commandLine.push_back(command1);

            COMBINATIONS("Set a multiple command lines") {
                rootSettings.clear(copyAndAppend(baseSettingsKeys, "command-line"));
                rootSettings.add(combineVectors(baseSettingsKeys, {"command-line", "commandA"}), command1);
                rootSettings.add(combineVectors(baseSettingsKeys, {"command-line", "commandB"}), command2);
                commandLine.push_back(command2);
            }

            COMBINATIONS("Set environment") {
                setEnvironment(baseSettingsKeys, &expectedTask, &rootSettings, {{"VAR1", "environmentValue{" + compilerUtil.compiler.getKey() + "}"}, {"VAR{" + compilerUtil.compiler.getKey() + "}", "environmentValue2"}});
            }

            ExecutorStub::TaskQueue expectedTasks;
            for(const auto& commandLine : commandLine) {
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
            rootSettings.add({PLUGIN_CONFIG_KEY}, command);

            Task task;
            CommandLineCommand plugin;

            WHEN("We add no parameter and apply") {
                bool return_code = plugin.apply(command, task, options);

                THEN("The call should not succeed") {
                    REQUIRE_FALSE(return_code);
                }
            }

            WHEN("We add no parameter and apply") {
                rootSettings.add({PLUGIN_CONFIG_KEY}, "command-line");
                bool return_code = plugin.apply(command, task, options);

                THEN("The call should not succeed") {
                    REQUIRE_FALSE(return_code);
                }
            }

            WHEN("We add an other filled in command-line and no parameter for the right one and apply") {
                rootSettings.add({PLUGIN_CONFIG_KEY, "command-line"}, "random-value");
                rootSettings.add({PLUGIN_CONFIG_KEY, command}, "command-line");
                bool return_code = plugin.apply(command, task, options);

                THEN("The call should not succeed") {
                    REQUIRE_FALSE(return_code);
                }
            }
        }
    }
} } }
