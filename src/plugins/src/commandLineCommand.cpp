#include "commandLineCommand.h"

#include <string>
#include <vector>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"

#include "pluginUtils.h"
#include "configValue.h"

using std::string;
using std::vector;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::EnvironmentCollection;
using execHelper::core::PatternKeys;

namespace execHelper { namespace plugins {
    bool CommandLineCommand::apply(const std::string& command, Task& task, const Options& options) const noexcept {
        static string commandLineCommandKey("command-line-command");
        static string commandLineKey("command-line");

        const SettingsNode& rootSettings = options.getSettings(commandLineCommandKey);
        boost::optional<const SettingsNode&> commandLineSettingsOpt = ConfigValue<const SettingsNode&>::getSetting("command-line", rootSettings, {{command}, {}});
        if(commandLineSettingsOpt == boost::none) {
            user_feedback_error("Could not find the '" << getCommandLineKey() << "' setting for command '" << command << "' in the '" << commandLineCommandKey << "' settings");
            return false;
        }
        const SettingsNode& commandLineSettings = commandLineSettingsOpt.get();
        if(commandLineSettings.m_values.empty()) {
            user_feedback_error("The '" << getCommandLineKey() << "' setting for command '" << command << "' in the '" << commandLineCommandKey << "' settings is empty");
            return false;
        }

        vector<Task> tasks;
        if(commandLineSettings.m_values.front().m_values.empty()) {
            // It is a list of commands that is together one command
            Task newTask = task;
            newTask.append(getCommandLine(command, rootSettings));
            tasks.emplace_back(newTask);
        } else {
            for(const auto& commandLine : commandLineSettings.m_values) {
                Task newTask = task;
                TaskCollection newTaskCollection = ConfigValue<TaskCollection>::get(commandLine.m_key, {}, commandLineSettings, {{}});
                newTask.append(newTaskCollection);
                tasks.emplace_back(newTask);
            }
        }

        EnvironmentCollection environment;
        boost::optional<const SettingsNode&> environmentSettings = ConfigValue<const SettingsNode&>::getSetting("environment", rootSettings, {{command}, {}});
        if(environmentSettings != boost::none) {
            for(const auto& setting : environmentSettings.get().m_values) {
                if(!setting.m_values.empty()) {
                    for(auto& subtask : tasks) {
                        subtask.appendToEnvironment(make_pair(setting.m_key, setting.m_values.back().m_key));
                    }
                }
            }
        }

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            for(const auto& newTask : tasks) {
                Task commandLineTask = replacePatternCombinations(newTask, combination);
                registerTask(commandLineTask, options);
            }
        }
        return true;
    }
} }
