#include <string>
#include <fstream>

#include "config/settingsNode.h"
#include "plugins/pluginUtils.h"
#include "plugins/make.h"

#include "utils/utils.h"
#include "unittest/catch.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::string;
using std::ofstream;

using execHelper::config::SettingsNode;
using execHelper::core::test::ExecutorStub;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CommandCollection;

using execHelper::test::OptionsStub;
using execHelper::test::utils::copyAndAppend;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::addPatterns;
using execHelper::test::utils::getExpectedTasks;

namespace {
    const string pluginConfigKey("make");
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the plugin configuration settings", "[plugins][make]") {
        MAKE_COMBINATIONS("Of several settings") {
            const string command("make-command");

            OptionsStub options;

            TargetUtil targetUtil;
            addPatterns(targetUtil.getPatterns(), options);

            CompilerUtil compilerUtil;
            addPatterns(compilerUtil.getPatterns(), options);

            SettingsNode& rootSettings = options.m_settings;
            rootSettings.add({pluginConfigKey, "patterns"}, targetUtil.getKeys());
            rootSettings.add({pluginConfigKey, "patterns"}, compilerUtil.getKeys());

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");

            Make plugin;
            Task task;

            Task expectedTask({"make"});
            TaskCollection jobs({"--jobs", "8"});
            TaskCollection buildDir;
            TaskCollection verbosity;
            TaskCollection commandLine;

            SettingsNode::SettingsKeys baseSettingsKeys = {pluginConfigKey};
            SettingsNode::SettingsKeys otherBaseSettingsKeys = {pluginConfigKey, otherCommandKey};

            COMBINATIONS("Toggle between general and specific command settings") {
                baseSettingsKeys.push_back(command);
            }

            COMBINATIONS("Switch off multi-threading") {
                rootSettings.add(copyAndAppend(baseSettingsKeys, "single-threaded"), "yes");
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "single-threaded"), "no");
                jobs.clear();
            }

            COMBINATIONS("Switch on multi-threading") {
                rootSettings.add(copyAndAppend(baseSettingsKeys, "single-threaded"), "no");     // Last one counts
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "single-threaded"), "yes");
                jobs = TaskCollection({"--jobs", "8"});
            }

            COMBINATIONS("Switch off the multi-threading option") {
                options.m_singleThreaded = true;
                jobs.clear();
            }

            COMBINATIONS("Set a build directory") {
                const string buildDirValue("{" + compilerUtil.distribution.getKey() + "}/{" + compilerUtil.architecture.getKey() + "}/{HELLO}/{" + compilerUtil.compiler.getKey() + "}/hello{" + compilerUtil.mode.getKey() + " }world"); 
                rootSettings.add(copyAndAppend(baseSettingsKeys, "build-dir"), buildDirValue);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "build-dir"), "some/build/dir");
                buildDir.emplace_back("--directory=" + buildDirValue);
            }

            COMBINATIONS("Switch off verbosity") {
                options.m_verbosity = false;
            }

            COMBINATIONS("Switch on verbosity") {
                options.m_verbosity = true;
                verbosity.emplace_back("--debug");
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + targetUtil.target.getKey() + "}", "{" + targetUtil.runTarget.getKey() + "}"};
                rootSettings.add(copyAndAppend(baseSettingsKeys, "command-line"), commandLine);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "command-line"), "--some-command");
            }

            expectedTask.append(jobs);
            expectedTask.append(buildDir);
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
