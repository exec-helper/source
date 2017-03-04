#include <string>

#include "config/settingsNode.h"

#include "unittest/catch.h"
#include "utils/utils.h"

#include "plugins/clangTidy.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

using execHelper::test::OptionsStub;
using execHelper::test::utils::addPatterns;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::toString;
using execHelper::core::test::ExecutorStub;

namespace {
    const string PLUGIN_CONFIG_KEY("clang-tidy");
    const string PLUGIN_COMMAND(PLUGIN_CONFIG_KEY);
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the configuration settings of the clang-tidy plugin", "[plugins][clang-tidy]") {
        MAKE_COMBINATIONS("Of several settings") {
            const string command("clang-tidy-command");

            OptionsStub options;

            TargetUtil targetUtil;
            addPatterns(targetUtil.getPatterns(), options);

            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, command);
            addSettings(rootSettings[PLUGIN_CONFIG_KEY], "patterns", targetUtil.getKeys());

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, otherCommandKey);

            ClangTidy plugin;
            Task task;

            Task expectedTask({PLUGIN_COMMAND});
            TaskCollection commandLine;
            TaskCollection sources;
            TaskCollection checks;

            SettingsNode* settings = &(rootSettings[PLUGIN_CONFIG_KEY]);

            COMBINATIONS("Toggle between general and specific command settings") {
                settings = &rootSettings[PLUGIN_CONFIG_KEY][command];
            }

            COMBINATIONS("Add sources") {
                sources.emplace_back("source1/{" + targetUtil.target.getKey() + "}");
                sources.emplace_back("source2");
                sources.emplace_back("source3/{" + targetUtil.runTarget.getKey() + "}");
                addSettings(*settings, "sources", sources);
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "sources", {"source4", "source5"});
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + targetUtil.target.getKey() + "}", "{" + targetUtil.runTarget.getKey() + "}"};
                addSettings(*settings, "command-line", commandLine);
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "command-line", "--some-command");
            }

            COMBINATIONS("Add command line to source") {
                if(settings->contains("sources")) {
                    if((*settings)["sources"].contains("source2")) {
                        commandLine.clear(); 
                        commandLine = {"{" + targetUtil.target.getKey() + "}", "{" + targetUtil.runTarget.getKey() + "}"};

                        addSettings((*settings)["sources"]["source1/{" + targetUtil.target.getKey() + "}"], "command-line", commandLine);
                        addSettings((*settings)["sources"]["source2"], "command-line", commandLine);
                        addSettings((*settings)["sources"]["source3/{" + targetUtil.runTarget.getKey() + "}"], "command-line", commandLine);
                        addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "command-line", "--some-command");
                    }
                }
            }

            COMBINATIONS("Add checks") {
                TaskCollection checkValue = {"check1", "check2-{" + targetUtil.target.getKey() + "}"};
                addSettings(rootSettings[PLUGIN_CONFIG_KEY], "checks", checkValue);
                addSettings(rootSettings[otherCommandKey], "checks", {"check3"});
                checks.emplace_back("-checks=check1,check2-{" + targetUtil.target.getKey() + "}");
            }

            COMBINATIONS("Add checks to source") {
                if(settings->contains("sources")) {
                    if((*settings)["sources"].contains("source2")) {
                        checks.clear();

                        TaskCollection checkValue = {"check1", "check2-{" + targetUtil.target.getKey() + "}"};
                        addSettings((*settings)["sources"]["source1/{" + targetUtil.target.getKey() + "}"], "checks", checkValue);
                        addSettings((*settings)["sources"]["source2"], "checks", checkValue);
                        addSettings((*settings)["sources"]["source3/{" + targetUtil.runTarget.getKey() + "}"], "checks", checkValue);
                        addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "checks", {"check3"});
                        checks.emplace_back("-checks=check1,check2-{" + targetUtil.target.getKey() + "}");
                    }
                }
            }

            ExecutorStub::TaskQueue expectedTasks;
            for(const auto& source : sources) {
                Task sourceTask = expectedTask;
                sourceTask.append(checks);
                sourceTask.append(commandLine);
                sourceTask.append(source);
                const ExecutorStub::TaskQueue sourceTasks = getExpectedTasks(sourceTask, targetUtil);
                expectedTasks.insert(expectedTasks.end(), sourceTasks.begin(), sourceTasks.end());
            }

            if(! sources.empty()) { 
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
