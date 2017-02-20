#include <vector>
#include <string>
#include <fstream>

#include "config/settingsNode.h"
#include "plugins/pluginUtils.h"
#include "plugins/make.h"

#include "utils/utils.h"
#include "unittest/catch.h"

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
using execHelper::plugins::replacePatternCombinations;

using execHelper::test::OptionsStub;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::TargetUtilNames;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::CompilerUtilNames;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::addPatterns;

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
            addSettings(rootSettings, pluginConfigKey, command);
            addSettings(rootSettings[pluginConfigKey], "patterns", targetUtil.getKeys());
            addSettings(rootSettings[pluginConfigKey], "patterns", compilerUtil.getKeys());

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");
            addSettings(rootSettings, pluginConfigKey, otherCommandKey);

            Make plugin;
            Task task;

            Task expectedTask({"make"});
            TaskCollection jobs({"--jobs", "8"});
            TaskCollection buildDir;
            TaskCollection verbosity;
            TaskCollection commandLine;

            SettingsNode* settings = &(rootSettings[pluginConfigKey]);

            COMBINATIONS("Toggle between general and specific command settings") {
                settings = &rootSettings[pluginConfigKey][command];
            }

            COMBINATIONS("Switch off multi-threading") {
                addSettings(*settings, "single-threaded", "yes");
                addSettings(rootSettings[pluginConfigKey][otherCommandKey], "single-threaded", "no");
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
                    addSettings(rootSettings[pluginConfigKey][otherCommandKey], "single-threaded", "yes");
                }
                jobs = TaskCollection({"--jobs", "8"});
            }

            COMBINATIONS("Switch off the multi-threading option") {
                options.m_singleThreaded = true;
                jobs.clear();
            }

            COMBINATIONS("Set a build directory") {
                const string buildDirValue("{" + compilerUtil.distribution.getKey() + "}/{" + compilerUtil.architecture.getKey() + "}/{HELLO}/{" + compilerUtil.compiler.getKey() + "}/hello{" + compilerUtil.mode.getKey() + " }world"); 
                addSettings(*settings, "build-dir", buildDirValue);
                addSettings(rootSettings[pluginConfigKey][otherCommandKey], "build-dir", "some/build/dir");
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
                addSettings(*settings, "command-line", commandLine);
                addSettings(rootSettings[pluginConfigKey][otherCommandKey], "command-line", "--some-command");
            }

            expectedTask.append(jobs);
            expectedTask.append(buildDir);
            expectedTask.append(verbosity);
            expectedTask.append(commandLine);

            ExecutorStub::TaskQueue expectedTasks;
            for(const auto& compiler : compilerUtil.makePatternPermutator()) {
                for(const auto& target : targetUtil.makePatternPermutator()) {
                    Task replacedExpectedTask = replacePatternCombinations(expectedTask, compiler);
                    replacedExpectedTask = replacePatternCombinations(replacedExpectedTask, target);
                    expectedTasks.emplace_back(replacedExpectedTask);
                }
            }

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
