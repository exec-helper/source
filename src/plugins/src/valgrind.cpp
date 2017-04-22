#include "valgrind.h"

#include <string>

#include "config/settingsNode.h"
#include "core/patterns.h"
#include "core/task.h"
#include "log/log.h"

#include "executePlugin.h"
#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;

namespace execHelper { namespace plugins {
    bool Valgrind::apply(const std::string& command, Task& task, const Options& options) const noexcept {
        static const string valgrindCommand("valgrind");
        task.append(valgrindCommand);

        const SettingsNode& rootSettings = options.getSettings({"valgrind"});

        static const string runCommandConfigKey("run-command");
        boost::optional<string> runCommand = getConfigurationSetting(command, rootSettings, runCommandConfigKey);
        if(runCommand == boost::none) {
            user_feedback_error("Could not find the '" << runCommandConfigKey << "' setting for command '" << command << "' in the '" << valgrindCommand << "' settings");
            return false;
        }

        static const string toolConfigKey("tool");
        boost::optional<string> tool = getConfigurationSetting(command, rootSettings, toolConfigKey, "--tool=");
        if(tool != boost::none && !tool.get().empty()) {
            task.append(tool.get());
        }

        if(options.getVerbosity()) {
            task.append("--verbose");
        }

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task newTask = task;
            newTask.append(getCommandLine(command, rootSettings, combination)); 

            ExecutePlugin plugin({runCommand.get()});
            if(!plugin.apply(command, newTask, options)) {
                return false;
            }
        }
        return true;
    }
} }
