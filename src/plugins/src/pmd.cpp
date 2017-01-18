#include "pmd.h"

#include <string>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"

#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;

namespace execHelper { namespace plugins {
    bool Pmd::apply(const std::string& command, Task& task, const Options& options) const noexcept {
        const string pmdConfigKey("pmd");
        const SettingsNode& rootSettings = options.getSettings(pmdConfigKey);

        static string execConfigKey("exec");
        boost::optional<string> execCommand = getConfigurationSetting(command, rootSettings, execConfigKey);
        if(execCommand == boost::none) {
            user_feedback_error("Could not find the '" << execConfigKey << "' setting for command '" << command << "' in the '" << pmdConfigKey << "' settings");
            return false;
        }
        task.append(execCommand.get());

        static const string toolConfigKey("tool");
        boost::optional<string> tool = getConfigurationSetting(command, rootSettings, toolConfigKey);
        if(tool == boost::none) {
            user_feedback_error("Could not find the '" << toolConfigKey << "' setting for command '" << command << "' in the '" << pmdConfigKey << "' settings");
            return false;
        }
        task.append(tool.get());
        if(tool.get() == "cpd") {
            static const string minimumTokensConfigKey("minimum-tokens");
            boost::optional<string> minimumTokens = getConfigurationSetting(command, rootSettings, minimumTokensConfigKey);
            if(minimumTokens != boost::none) {
                task.append({"--minimum-tokens", minimumTokens.get()});
            }

            static const string filesConfigKey("files");
            boost::optional<TaskCollection> files = getConfigurationSettings(command, rootSettings, filesConfigKey);
            if(files != boost::none) {
                for(const auto& file : files.get()) {
                    task.append({"--files", file});
                }
            }
        }

        static const string languageConfigKey("language");
        boost::optional<string> language = getConfigurationSetting(command, rootSettings, languageConfigKey);
        if(language != boost::none) {
            task.append({"--language", language.get()});
        }

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task newTask = task;
            newTask.append(getCommandLine(command, rootSettings, combination)); 

            TaskCollection newTaskCollection = newTask.getTask();
            replacePatternCombinations(newTaskCollection, combination);
            Task replacedTask;
            replacedTask.append(std::move(newTaskCollection));

            if(! registerTask(replacedTask, options)) {
                return false;
            }
        }
        return true;
    }
} }
