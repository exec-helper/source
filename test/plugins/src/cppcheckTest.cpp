#include <vector>
#include <string>
#include <fstream>

#include "config/settingsNode.h"
#include "core/execHelperOptions.h"
#include "core/pattern.h"
#include "plugins/cppcheck.h"

#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::vector;
using std::string;
using std::ofstream;

using execHelper::config::SettingsNode;
using execHelper::core::test::ExecutorStub;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CommandCollection;
using execHelper::core::Pattern;

using execHelper::test::utils::addSettings;
using execHelper::test::OptionsStub;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::addPatterns;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::CompilerUtil;

namespace {
    const string PLUGIN_CONFIG_KEY("cppcheck");
    const string PLUGIN_COMMAND(PLUGIN_CONFIG_KEY);
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the configuration settings of the cppcheck plugin", "[plugins][cppcheck]") {
        MAKE_COMBINATIONS("Of several settings") {
            const string command("make-command");

            OptionsStub options;

            TargetUtil targetUtil;
            addPatterns(targetUtil.getPatterns(), options);

            CompilerUtil compilerUtil;
            addPatterns(compilerUtil.getPatterns(), options);

            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, command);
            addSettings(rootSettings[PLUGIN_CONFIG_KEY], "patterns", targetUtil.getKeys());
            addSettings(rootSettings[PLUGIN_CONFIG_KEY], "patterns", compilerUtil.getKeys());

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, otherCommandKey);

            Cppcheck plugin;
            Task task;

            Task expectedTask({PLUGIN_COMMAND});
            std::string enabledChecks("--enable=all");
            std::string srcDir(".");
            TaskCollection verbosity;
            TaskCollection commandLine;

            SettingsNode* settings = &(rootSettings[PLUGIN_CONFIG_KEY]);

            COMBINATIONS("Toggle between general and specific command settings") {
                settings = &rootSettings[PLUGIN_CONFIG_KEY][command];
            }

            COMBINATIONS("Change the enabled checks") {
                TaskCollection enabledChecksValue = {"warning", "style", "performance"}; 
                addSettings(*settings, "enable-checks", enabledChecksValue);
                enabledChecks.clear();
                enabledChecks = "--enable=warning,style,performance";
            }

            COMBINATIONS("Change the source dir") {
                srcDir.clear();
                srcDir = "{" + targetUtil.target.getKey() + "}/{HELLO}/{" + targetUtil.runTarget.getKey() + "}";
                addSettings(*settings, "src-dir", "{" + targetUtil.target.getKey() + "}/{HELLO}/{" + targetUtil.runTarget.getKey() + "}");
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "src-dir", "{" + targetUtil.target.getKey() + "}/{HELLO}/{" + targetUtil.runTarget.getKey() + "}");
            }

            COMBINATIONS("Change the target dir") {
                srcDir += "/{" + targetUtil.runTarget.getKey() + "}/{HELLO}";
                addSettings(*settings, "target-path", "{" + targetUtil.runTarget.getKey() + "}/{HELLO}");
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "target-path", "{" + targetUtil.runTarget.getKey() + "}/{HELLO}");
            }

            COMBINATIONS("Switch off verbosity") {
                options.m_verbosity = false;
            }

            COMBINATIONS("Switch on verbosity") {
                options.m_verbosity = true;
                verbosity.emplace_back("--verbose");
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + targetUtil.target.getKey() + "}", "{" + targetUtil.runTarget.getKey() + "}"};
                addSettings(*settings, "command-line", commandLine);
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "command-line", "--some-command");
            }

            expectedTask.append(enabledChecks);
            expectedTask.append(verbosity);
            expectedTask.append(commandLine);
            expectedTask.append(srcDir);

            ExecutorStub::TaskQueue expectedTasks = getExpectedTasks(expectedTask, compilerUtil, targetUtil);

            bool returnCode = plugin.apply(command, task, options);
            THEN_CHECK("It should succeed") {
                REQUIRE(returnCode == true);
            }

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
            }
        }
    }
} } }
