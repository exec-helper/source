#include <vector>
#include <string>
#include <fstream>

#include "unittest/catch.h"

#include "core/pattern.h"
#include "plugins/scons.h"
#include "plugins/pluginUtils.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::vector;
using std::string;

using execHelper::plugins::replacePatternCombinations;
using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

using execHelper::core::test::ExecutorStub;
using execHelper::test::OptionsStub;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::TargetUtilNames;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::CompilerUtilNames;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::addPatterns;

namespace {
    const string PLUGIN_CONFIG_KEY("scons");
    const string PLUGIN_COMMAND("scons");
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the configuration settings of the scons plugin", "[plugins][scons]") {
        MAKE_COMBINATIONS("Of several settings") {
            const string command("scons-command");

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

            Scons plugin;
            Task task;

            Task expectedTask({PLUGIN_COMMAND});
            TaskCollection jobs({"--jobs", "8"});
            TaskCollection verbosity;
            TaskCollection commandLine;

            SettingsNode* settings = &(rootSettings[PLUGIN_CONFIG_KEY]);

            COMBINATIONS("Toggle between general and specific command settings") {
                settings = &rootSettings[PLUGIN_CONFIG_KEY][command];
            }

            COMBINATIONS("Switch off multi-threading") {
                addSettings(*settings, "single-threaded", "yes");
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "single-threaded", "no");
                jobs.clear();
            }

            COMBINATIONS("Switch on multi-threading") {
                // Note: we may be overruling the option above
                if(settings->contains("single-threaded")) {
                    (*settings)["single-threaded"].m_values.clear();
                    SettingsNode newNode;
                    newNode.m_key = "no";
                    (*settings)["single-threaded"].m_values.emplace_back(newNode);
                } else {
                    addSettings(*settings, "single-threaded", "no");
                    addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "single-threaded", "yes");
                }
                jobs = TaskCollection({"--jobs", "8"});
            }

            COMBINATIONS("Switch off the multi-threading option") {
                options.m_singleThreaded = true;
                jobs.clear();
            }

            COMBINATIONS("Switch off verbosity") {
                options.m_verbosity = false;
            }

            COMBINATIONS("Switch on verbosity") {
                options.m_verbosity = true;
                verbosity.emplace_back("--debug=explain");
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}", "blaat/{HELLO}/{" + compilerUtil.compiler.getKey() + "}"};
                addSettings(*settings, "command-line", commandLine);
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "command-line", "--some-command");
            }

            expectedTask.append(jobs);
            expectedTask.append(verbosity);
            expectedTask.append(commandLine);

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
