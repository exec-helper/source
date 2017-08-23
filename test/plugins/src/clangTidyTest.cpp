#include <map>
#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/settingsNode.h"
#include "plugins/clangTidy.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::string;
using std::vector;
using std::map;

using gsl::czstring;

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
    const czstring<> PLUGIN_CONFIG_KEY("clang-tidy");
    const czstring<> PLUGIN_COMMAND("clang-tidy");
    const czstring<> SOURCES_KEY("sources");
    const czstring<> WARNING_AS_ERROR_KEY("warnings-as-errors");
    const czstring<> WARNING_AS_ERROR_INHERIT_KEY("all");

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
        string result="-warnings-as-errors=";
        for(size_t i = 0; i < taskCollection.size() - 1; ++i) {
            result += taskCollection[i] + ",";
        }
        result += taskCollection.back();
        return TaskCollection({result});
    }

    inline void inheritChecks(const TaskCollection& sourceKeys, TaskCollection* warningAsError, map<string, TaskCollection>* sourceSpecificWarningAsError, const TaskCollection& checks, const map<string, TaskCollection>& sourceSpecificChecks) {
        static const string toReplace("-checks=");
        if(!sourceSpecificChecks.empty()) {
            *sourceSpecificWarningAsError = sourceSpecificChecks;
            for(auto& entry : *sourceSpecificWarningAsError) {
                for(auto& parameter : entry.second) {
                    size_t pos = parameter.find(toReplace);
                    parameter.replace(pos, toReplace.size(), "-warnings-as-errors=");
                }
            }
        } else {
            *warningAsError = checks;
            for(auto& parameter : *warningAsError) {
                size_t pos = parameter.find(toReplace);
                parameter.replace(pos, toReplace.size(), "-warnings-as-errors=");
            }
            for(const auto& sourceKey : sourceKeys) {
                (*sourceSpecificWarningAsError)[sourceKey] = *warningAsError;
            }
        }
    }
} // namespace

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

            addPatterns(targetUtil.getPatterns(), &options);

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
            TaskCollection warningAsError;
            map<string, TaskCollection> sourceSpecificWarningAsError;

            SettingsNode::SettingsKeys baseSettingsKeys = {PLUGIN_CONFIG_KEY};
            SettingsNode::SettingsKeys otherBaseSettingsKeys = {PLUGIN_CONFIG_KEY, otherCommandKey};

            COMBINATIONS("Toggle between general and specific command settings") {
                baseSettingsKeys.push_back(command);
            }

            COMBINATIONS("Add sources") {
                sources = sourceKeys;
                rootSettings.add(copyAndAppend(baseSettingsKeys, SOURCES_KEY), sources);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, SOURCES_KEY), {"source4", "source5"});
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + targetUtil.target.getKey() + "}", "{" + targetUtil.runTarget.getKey() + "}"};
                rootSettings.add(copyAndAppend(baseSettingsKeys, "command-line"), commandLine);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "command-line"), "--some-command");
            }

            COMBINATIONS("Add command line to source") {
                if(rootSettings.contains(combineVectors(baseSettingsKeys, {SOURCES_KEY, "source2"}))) {
                    const map<string, TaskCollection> commandLineValues = {
                        {sourceKey1, {"{" + targetUtil.target.getKey() + "}"}}, 
                        {sourceKey2, {"{" + targetUtil.runTarget.getKey() + "}"}}, 
                        {sourceKey3, {"{" + targetUtil.target.getKey() + "}", "{" + targetUtil.runTarget.getKey() + "}"}}
                    };

                    for(const auto& sourceKey : sourceKeys) {
                        rootSettings.add(combineVectors(baseSettingsKeys, {SOURCES_KEY, sourceKey, "command-line"}), commandLineValues.at(sourceKey));
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
                if(rootSettings.contains(combineVectors(baseSettingsKeys, {SOURCES_KEY, "source2"}))) {
                    const map<string, TaskCollection> checkValues = {
                        {sourceKey1, {"check1", "check2-{" + targetUtil.target.getKey() + "}"}}, 
                        {sourceKey2, {"check3", "check4-{" + targetUtil.runTarget.getKey() + "}"}}, 
                        {sourceKey3, {"check5", "check6-{" + targetUtil.target.getKey() + "}"}}
                    };

                    for(const auto& sourceKey : sourceKeys) {
                        rootSettings.add(combineVectors(baseSettingsKeys, {SOURCES_KEY, sourceKey, "checks"}), checkValues.at(sourceKey));
                        sourceSpecificChecks[sourceKey] = toChecks(checkValues.at(sourceKey));
                    }
                    rootSettings.add({PLUGIN_CONFIG_KEY, "checks"}, "check7");
                    rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "checks"), "check8");
                }
            }

            COMBINATIONS("Add warning as error") {
                const TaskCollection warningAsErrorValue = {"warningAsError1", "warningAsError2", "warningAsError3"};
                rootSettings.add(copyAndAppend(baseSettingsKeys, WARNING_AS_ERROR_KEY), warningAsErrorValue);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, WARNING_AS_ERROR_KEY), {"warningAsError4"});
                warningAsError.emplace_back("-warnings-as-errors=warningAsError1,warningAsError2,warningAsError3");
            }

            COMBINATIONS("Add warning as error to source") {
                const map<string, TaskCollection> warningAsErrorValues = {
                    {sourceKey1, {"warningAsError1", "warningAsError2-{" + targetUtil.target.getKey() + "}"}}, 
                    {sourceKey2, {"warningAsError3", "warningAsError4-{" + targetUtil.runTarget.getKey() + "}"}}, 
                    {sourceKey3, {"warningAsError5", "warningAsError6-{" + targetUtil.target.getKey() + "}"}}
                };

                for(const auto& sourceKey : sourceKeys) {
                    rootSettings.add(combineVectors(baseSettingsKeys, {SOURCES_KEY, sourceKey, WARNING_AS_ERROR_KEY}), warningAsErrorValues.at(sourceKey));
                    sourceSpecificWarningAsError[sourceKey] = toWarningAsError(warningAsErrorValues.at(sourceKey));
                }
                rootSettings.add({PLUGIN_CONFIG_KEY, WARNING_AS_ERROR_KEY}, {"warning-as-errorA"});
                rootSettings.add({PLUGIN_CONFIG_KEY, otherCommandKey, WARNING_AS_ERROR_KEY}, {"warning-as-errorB"});
            }

            COMBINATIONS("Add general all checks") {
                rootSettings.clear(copyAndAppend(baseSettingsKeys, WARNING_AS_ERROR_KEY));
                for(const auto& sourceKey : sourceKeys) {
                    rootSettings.clear(combineVectors(baseSettingsKeys, {SOURCES_KEY, sourceKey, WARNING_AS_ERROR_KEY}));
                    sourceSpecificWarningAsError[sourceKey].clear();
                }

                rootSettings.add(copyAndAppend(baseSettingsKeys, WARNING_AS_ERROR_KEY), WARNING_AS_ERROR_INHERIT_KEY);

                inheritChecks(sourceKeys, &warningAsError, &sourceSpecificWarningAsError, checks, sourceSpecificChecks);
            }

            COMBINATIONS("Add all checks as source specific warning as error") {
                for(const auto& sourceKey : sourceKeys) {
                    rootSettings.clear(combineVectors(baseSettingsKeys, {SOURCES_KEY, sourceKey, WARNING_AS_ERROR_KEY}));
                    sourceSpecificWarningAsError[sourceKey].clear();

                    rootSettings.add(combineVectors(baseSettingsKeys, {SOURCES_KEY, sourceKey, WARNING_AS_ERROR_KEY}), WARNING_AS_ERROR_INHERIT_KEY);
                    rootSettings.add(combineVectors(otherBaseSettingsKeys, {SOURCES_KEY, sourceKey, WARNING_AS_ERROR_KEY}), {"warning-as-errorA"});
                }

                inheritChecks(sourceKeys, &warningAsError, &sourceSpecificWarningAsError, checks, sourceSpecificChecks);
            }

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
} // namespace test
} // namespace plugins
} // namespace execHelper
