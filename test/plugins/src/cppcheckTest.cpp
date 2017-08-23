#include <string>

#include <gsl/string_span>

#include "config/settingsNode.h"
#include "core/execHelperOptions.h"
#include "core/pattern.h"
#include "plugins/cppcheck.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::string;

using gsl::czstring;

using execHelper::config::SettingsNode;
using execHelper::core::test::ExecutorStub;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CommandCollection;

using execHelper::test::OptionsStub;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::addPatterns;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::copyAndAppend;

namespace {
    const czstring<> PLUGIN_CONFIG_KEY("cppcheck");
    const czstring<> PLUGIN_COMMAND("cppcheck");
} // namespace

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the configuration settings of the cppcheck plugin", "[plugins][cppcheck]") {
        MAKE_COMBINATIONS("Of several settings") {
            const string command("make-command");

            OptionsStub options;

            TargetUtil targetUtil;
            addPatterns(targetUtil.getPatterns(), &options);

            CompilerUtil compilerUtil;
            addPatterns(compilerUtil.getPatterns(), &options);

            SettingsNode& rootSettings = options.m_settings;
            rootSettings.add({PLUGIN_CONFIG_KEY, "patterns"}, targetUtil.getKeys());
            rootSettings.add({PLUGIN_CONFIG_KEY, "patterns"}, compilerUtil.getKeys());

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");

            Cppcheck plugin;
            Task task;

            Task expectedTask({PLUGIN_COMMAND});
            std::string enabledChecks("--enable=all");
            std::string srcDir(".");
            TaskCollection verbosity;
            TaskCollection commandLine;

            SettingsNode::SettingsKeys baseSettingsKeys = {PLUGIN_CONFIG_KEY};
            SettingsNode::SettingsKeys otherBaseSettingsKeys = {PLUGIN_CONFIG_KEY, otherCommandKey};

            COMBINATIONS("Toggle between general and specific command settings") {
                baseSettingsKeys.push_back(command);
            }

            COMBINATIONS("Change the enabled checks") {
                TaskCollection enabledChecksValue = {"warning", "style", "performance"}; 
                rootSettings.add(copyAndAppend(baseSettingsKeys, "enable-checks"), enabledChecksValue);
                enabledChecks.clear();
                enabledChecks = "--enable=warning,style,performance";
            }

            COMBINATIONS("Change the source dir") {
                srcDir.clear();
                srcDir = "{" + targetUtil.target.getKey() + "}/{HELLO}/{" + targetUtil.runTarget.getKey() + "}";
                rootSettings.add(copyAndAppend(baseSettingsKeys, "src-dir"), srcDir);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "src-dir"), "other-src-dir");
            }

            COMBINATIONS("Change the target dir") {
                const string target("{" + targetUtil.runTarget.getKey() + "}/{HELLO}");
                srcDir +=  "/" + target;
                rootSettings.add(copyAndAppend(baseSettingsKeys, "target-path"), target);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "target-path"), "other-target");
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
                rootSettings.add(copyAndAppend(baseSettingsKeys, "command-line"), commandLine);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "command-line"), "--other-command");
            }

            expectedTask.append(enabledChecks);
            expectedTask.append(verbosity);
            expectedTask.append(commandLine);
            expectedTask.append(srcDir);

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
} // namespace test
} // namespace plugins
} // namespace execHelper
