#include <string>

#include "core/pattern.h"
#include "plugins/pluginUtils.h"
#include "plugins/scons.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

using execHelper::core::test::ExecutorStub;
using execHelper::test::OptionsStub;
using execHelper::test::utils::copyAndAppend;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::CompilerUtil;
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
            rootSettings.add({PLUGIN_CONFIG_KEY, "patterns"}, targetUtil.getKeys());
            rootSettings.add({PLUGIN_CONFIG_KEY, "patterns"}, compilerUtil.getKeys());

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");

            Scons plugin;
            Task task;

            Task expectedTask({PLUGIN_COMMAND});
            TaskCollection jobs({"--jobs", "8"});
            TaskCollection verbosity;
            TaskCollection commandLine;

            SettingsNode::SettingsKeys baseSettingsKeys = {PLUGIN_CONFIG_KEY};
            SettingsNode::SettingsKeys otherBaseSettingsKeys = {PLUGIN_CONFIG_KEY, otherCommandKey};

            COMBINATIONS("Toggle between general and specific command settings") {
                baseSettingsKeys.push_back(command);
            }

            COMBINATIONS("Switch on multi-threading") {
                // Note: we may be overruling the option above
                rootSettings.add(copyAndAppend(baseSettingsKeys, "single-threaded"), "no");     // Last value counts
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "single-threaded"), "yes");
                jobs = TaskCollection({"--jobs", "8"});
            }

            COMBINATIONS("Switch off multi-threading") {
                rootSettings.add(copyAndAppend(baseSettingsKeys, "single-threaded"), "yes");
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "single-threaded"), "no");
                jobs.clear();
            }

            COMBINATIONS("Switch off the multi-threading option") {
                options.m_singleThreaded = true;
                jobs.clear();
            }

            COMBINATIONS("Switch off verbosity") {
                options.m_verbosity = false;
            }

            COMBINATIONS("Switch on verbosity") {
                verbosity.emplace_back("--debug=explain");
                options.m_verbosity = true;
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}", "blaat/{HELLO}/{" + compilerUtil.compiler.getKey() + "}"};

                rootSettings.add(copyAndAppend(baseSettingsKeys, "command-line"), commandLine);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "command-line"), "--some-command");
            }

            expectedTask.append(jobs);
            expectedTask.append(verbosity);
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
} } }
