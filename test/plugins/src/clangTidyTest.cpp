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
using execHelper::test::utils::copyAndAppend;
using execHelper::test::utils::combineVectors;
using execHelper::core::test::ExecutorStub;

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

            OptionsStub options;

            addPatterns(targetUtil.getPatterns(), options);

            SettingsNode& rootSettings = options.m_settings;
            rootSettings.add({PLUGIN_CONFIG_KEY, "patterns"}, targetUtil.getKeys());

            ClangTidy plugin;
            Task task;

            Task expectedTask({PLUGIN_COMMAND});
            TaskCollection commandLine;
            map<string, TaskCollection> sourceSpecificCommandLine;
            TaskCollection sources;
            TaskCollection checks;
            map<string, TaskCollection> sourceSpecificChecks;

            SettingsNode::SettingsKeys baseSettingsKeys = {PLUGIN_CONFIG_KEY};
            SettingsNode::SettingsKeys otherBaseSettingsKeys = {PLUGIN_CONFIG_KEY, otherCommandKey};

            COMBINATIONS("Toggle between general and specific command settings") {
                baseSettingsKeys.push_back(command);
            }

            COMBINATIONS("Add sources") {
                sources = sourceKeys;
                rootSettings.add(copyAndAppend(baseSettingsKeys, "sources"), sources);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "sources"), {"source4", "source5"});
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + targetUtil.target.getKey() + "}", "{" + targetUtil.runTarget.getKey() + "}"};
                rootSettings.add(copyAndAppend(baseSettingsKeys, "command-line"), commandLine);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "command-line"), "--some-command");
            }

            COMBINATIONS("Add command line to source") {
                if(rootSettings.contains(combineVectors(baseSettingsKeys, {"sources", "source2"}))) {
                    const map<string, TaskCollection> commandLineValues = {
                        {sourceKey1, {"{" + targetUtil.target.getKey() + "}"}}, 
                        {sourceKey2, {"{" + targetUtil.runTarget.getKey() + "}"}}, 
                        {sourceKey3, {"{" + targetUtil.target.getKey() + "}", "{" + targetUtil.runTarget.getKey() + "}"}}
                    };

                    for(const auto& sourceKey : sourceKeys) {
                        rootSettings.add(combineVectors(baseSettingsKeys, {"sources", sourceKey, "command-line"}), commandLineValues.at(sourceKey));
                        sourceSpecificCommandLine[sourceKey] = commandLineValues.at(sourceKey);
                    }
                    rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "command-line"), "--some-command");
                    rootSettings.add({PLUGIN_CONFIG_KEY, "command-line"}, "--some-command");
                }
            }

            COMBINATIONS("Add checks") {
                TaskCollection checkValue = {"check1", "check2-{" + targetUtil.target.getKey() + "}"};
                rootSettings.add(copyAndAppend(baseSettingsKeys, "checks"), checkValue);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "checks"), "check3");
                checks.emplace_back("-checks=check1,check2-{" + targetUtil.target.getKey() + "}");
            }

            COMBINATIONS("Add checks to source") {
                if(rootSettings.contains(combineVectors(baseSettingsKeys, {"sources", "source2"}))) {
                    const map<string, TaskCollection> checkValues = {
                        {sourceKey1, {"check1", "check2-{" + targetUtil.target.getKey() + "}"}}, 
                        {sourceKey2, {"check3", "check4-{" + targetUtil.runTarget.getKey() + "}"}}, 
                        {sourceKey3, {"check5", "check6-{" + targetUtil.target.getKey() + "}"}}
                    };

                    for(const auto& sourceKey : sourceKeys) {
                        rootSettings.add(combineVectors(baseSettingsKeys, {"sources", sourceKey, "checks"}), checkValues.at(sourceKey));
                        sourceSpecificChecks[sourceKey] = toChecks(checkValues.at(sourceKey));
                    }
                    rootSettings.add({PLUGIN_CONFIG_KEY, "checks"}, "check7");
                    rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "checks"), "check8");
                }
            }

            ExecutorStub::TaskQueue expectedTasks;
            for(const auto& source : sources) {
                Task sourceTask = expectedTask;
                if(sourceSpecificChecks.count(source) > 0) {
                    sourceTask.append(sourceSpecificChecks[source]);
                } else {
                    sourceTask.append(checks);
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
