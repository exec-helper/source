#include <string>
#include <map>
#include <vector>

#include "config/settingsNode.h"

#include "unittest/catch.h"
#include "utils/utils.h"

#include "plugins/clangTidy.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::string;
using std::vector;
using std::map;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

using execHelper::test::OptionsStub;
using execHelper::test::utils::addPatterns;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::addSettings;
using execHelper::core::test::ExecutorStub;

using execHelper::test::utils::toString;

namespace {
    const string PLUGIN_CONFIG_KEY("clang-tidy");
    const string PLUGIN_COMMAND("clang-tidy");

    TaskCollection toChecks(const TaskCollection& taskCollection) noexcept {
        if(taskCollection.empty()) {
            return TaskCollection();
        }
        string result="-checks=";
        for(size_t i = 0; i < taskCollection.size() - 1; ++i) {
            result += taskCollection[i] + ",";
        }
        result += taskCollection.back();
        return TaskCollection({result});
    }

    TaskCollection toWarningAsError(const TaskCollection& taskCollection) noexcept {
        if(taskCollection.empty()) {
            return TaskCollection();
        }
        string result="-warning-as-error=";
        for(size_t i = 0; i < taskCollection.size() - 1; ++i) {
            result += taskCollection[i] + ",";
        }
        result += taskCollection.back();
        return TaskCollection({result});
    }
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the configuration settings of the clang-tidy plugin", "[plugins][clang-tidy]") {
        const string command("clang-tidy-command");
        const string otherCommandKey("other-command");

        const TargetUtil targetUtil;

        const string sourceKey1("source1/{" + targetUtil.target.getKey() + "}");
        const string sourceKey2("source2");
        const string sourceKey3("source3/{" + targetUtil.runTarget.getKey() + "}");
        const vector<string> sourceKeys({sourceKey1, sourceKey2, sourceKey3});

        MAKE_COMBINATIONS("Of several settings") {
            std::cout << make_combinations_index << std::endl;
            OptionsStub options;

            addPatterns(targetUtil.getPatterns(), options);

            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, command);
            addSettings(rootSettings[PLUGIN_CONFIG_KEY], "patterns", targetUtil.getKeys());

            // Add the settings of an other command to make sure we take the expected ones
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, otherCommandKey);

            ClangTidy plugin;
            Task task;

            Task expectedTask({PLUGIN_COMMAND});
            TaskCollection commandLine;
            map<string, TaskCollection> sourceSpecificCommandLine;
            TaskCollection sources;
            TaskCollection checks;
            map<string, TaskCollection> sourceSpecificChecks;
            TaskCollection warningAsError;
            map<string, TaskCollection> sourceSpecificWarningAsError;

            SettingsNode* tmpSettings = &(rootSettings[PLUGIN_CONFIG_KEY]);

            COMBINATIONS("Toggle between general and specific command settings") {
                tmpSettings = &(rootSettings[PLUGIN_CONFIG_KEY][command]);
            }
            SettingsNode* const settings = tmpSettings;

            COMBINATIONS("Add sources") {
                sources = sourceKeys;
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
                    if((*settings)["sources"].contains(sourceKey2)) {
                        const map<string, TaskCollection> commandLineValues = {
                            {sourceKey1, {"{" + targetUtil.target.getKey() + "}"}}, 
                            {sourceKey2, {"{" + targetUtil.runTarget.getKey() + "}"}}, 
                            {sourceKey3, {"{" + targetUtil.target.getKey() + "}", "{" + targetUtil.runTarget.getKey() + "}"}}
                        };

                        addSettings((*settings)["sources"][sourceKey1], "command-line", commandLineValues.at(sourceKey1));
                        addSettings((*settings)["sources"][sourceKey2], "command-line", commandLineValues.at(sourceKey2));
                        addSettings((*settings)["sources"][sourceKey3], "command-line", commandLineValues.at(sourceKey3));
                        addSettings(rootSettings[PLUGIN_CONFIG_KEY], "command-line", "--some-command");
                        addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "command-line", "--some-command");

                        sourceSpecificCommandLine[sourceKey1] = commandLineValues.at(sourceKey1);
                        sourceSpecificCommandLine[sourceKey2] = commandLineValues.at(sourceKey2);
                        sourceSpecificCommandLine[sourceKey3] = commandLineValues.at(sourceKey3);
                    }
                }
            }

            COMBINATIONS("Add checks") {
                TaskCollection checkValue = {"check1", "check2-{" + targetUtil.target.getKey() + "}"};
                addSettings(*settings, "checks", checkValue);
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "checks", {"check3"});
                checks.emplace_back("-checks=check1,check2-{" + targetUtil.target.getKey() + "}");
            }

            COMBINATIONS("Add checks to source") {
                if(settings->contains("sources")) {
                    if((*settings)["sources"].contains(sourceKey2)) {
                        const map<string, TaskCollection> checkValues = {
                            {sourceKey1, {"check1", "check2-{" + targetUtil.target.getKey() + "}"}}, 
                            {sourceKey2, {"check3", "check4-{" + targetUtil.runTarget.getKey() + "}"}}, 
                            {sourceKey3, {"check5", "check6-{" + targetUtil.target.getKey() + "}"}}
                        };

                        addSettings((*settings)["sources"][sourceKey1], "checks", checkValues.at(sourceKey1));
                        addSettings((*settings)["sources"][sourceKey2], "checks", checkValues.at(sourceKey2));
                        addSettings((*settings)["sources"][sourceKey3], "checks", checkValues.at(sourceKey3));
                        addSettings(rootSettings[PLUGIN_CONFIG_KEY], "checks", {"check7"});
                        addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "checks", {"check7"});

                        sourceSpecificChecks[sourceKey1] = toChecks(checkValues.at(sourceKey1));
                        sourceSpecificChecks[sourceKey2] = toChecks(checkValues.at(sourceKey2));
                        sourceSpecificChecks[sourceKey3] = toChecks(checkValues.at(sourceKey3));
                    }
                }
            }

            COMBINATIONS("Add warning as error") {
                const TaskCollection warningAsErrorValue = {"warningAsError1", "warningAsError2", "warningAsError3"};
                addSettings(*settings, "warning-as-error", warningAsErrorValue);
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "warning-as-error", {"warningAsError4"});
                warningAsError.emplace_back("-warning-as-error=warningAsError1,warningAsError2,warningAsError3");
            }

            COMBINATIONS("Add warning as error to source") {
                if(settings->contains("sources")) {
                    if((*settings)["sources"].contains(sourceKey2)) {
                        const map<string, TaskCollection> warningAsErrorValues = {
                            {sourceKey1, {"warningAsError1", "warningAsError2-{" + targetUtil.target.getKey() + "}"}}, 
                            {sourceKey2, {"warningAsError3", "warningAsError4-{" + targetUtil.runTarget.getKey() + "}"}}, 
                            {sourceKey3, {"warningAsError5", "warningAsError6-{" + targetUtil.target.getKey() + "}"}}
                        };

                        addSettings((*settings)["sources"][sourceKey1], "warning-as-error", warningAsErrorValues.at(sourceKey1));
                        addSettings((*settings)["sources"][sourceKey2], "warning-as-error", warningAsErrorValues.at(sourceKey2));
                        addSettings((*settings)["sources"][sourceKey3], "warning-as-error", warningAsErrorValues.at(sourceKey3));
                        addSettings(rootSettings[PLUGIN_CONFIG_KEY], "warning-as-error", {"warning-as-error"});
                        addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "warning-as-error", {"warning-as-error"});

                        sourceSpecificWarningAsError[sourceKey1] = toWarningAsError(warningAsErrorValues.at(sourceKey1));
                        sourceSpecificWarningAsError[sourceKey2] = toWarningAsError(warningAsErrorValues.at(sourceKey2));
                        sourceSpecificWarningAsError[sourceKey3] = toWarningAsError(warningAsErrorValues.at(sourceKey3));
                    }
                }
            }

            COMBINATIONS("Add all checks as warning as error") {
                (*settings)[PLUGIN_CONFIG_KEY]["warning-as-error"].m_values.clear();
                addSettings((*settings)[PLUGIN_CONFIG_KEY], "warning-as-error", "all");
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "warning-as-error", {"warningAsError4"});

                static const string toReplace("-checks=");
                if(!sourceSpecificChecks.empty()) {
                    sourceSpecificWarningAsError = sourceSpecificChecks;
                    for(auto& entry : sourceSpecificWarningAsError) {
                        for(auto& parameter : entry.second) {
                            size_t pos = parameter.find(toReplace);
                            parameter.replace(pos, toReplace.size(), "-warning-as-error=");
                        }
                    }
                } else {
                    warningAsError = checks;
                    for(auto& parameter : warningAsError) {
                        size_t pos = parameter.find(toReplace);
                        parameter.replace(pos, toReplace.size(), "-warning-as-error=");
                    }
                }
            }

            std::cout << toString(rootSettings) << std::endl;

            ExecutorStub::TaskQueue expectedTasks;
            for(const auto& source : sources) {
                Task sourceTask = expectedTask;
                if(sourceSpecificChecks.count(source) > 0) {
                    sourceTask.append(sourceSpecificChecks[source]);
                } else {
                    sourceTask.append(checks);
                }
                if(sourceSpecificWarningAsError.count(source) > 0) {
                    sourceTask.append(sourceSpecificWarningAsError[source]);
                } else {
                    sourceTask.append(warningAsError);
                }

                if(sourceSpecificCommandLine.count(source) > 0) {
                    sourceTask.append(sourceSpecificCommandLine[source]);
                } else {
                    sourceTask.append(commandLine);
                }
                sourceTask.append(source);
                const ExecutorStub::TaskQueue sourceTasks = getExpectedTasks(sourceTask, targetUtil);
                expectedTasks.insert(expectedTasks.end(), sourceTasks.begin(), sourceTasks.end());
            }

            if(!sources.empty()) {
                bool returnCode = plugin.apply(command, task, options);
                THEN_CHECK("It should succeed") {
                    REQUIRE(returnCode);
                }

                THEN_CHECK("It called the right commands") {
                    REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
                }
            }
        }
    }
} } }
