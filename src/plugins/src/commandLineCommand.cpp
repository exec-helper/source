#include "commandLineCommand.h"

#include <string>

#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"

#include "pluginUtils.h"

using std::string;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Patterns;
using execHelper::config::SettingsNode;

namespace execHelper { namespace plugins {
    bool CommandLineCommand::apply(const std::string& command, Task& task, const Options& options) const noexcept {
        static string commandLineCommandKey("command-line-command");
        static string commandLineKey("command-line");

        const SettingsNode& rootSettings = options.getSettings(commandLineCommandKey);
        const SettingsNode settings = getContainingSettings(command, rootSettings, commandLineKey); 
        TaskCollection tasks = settings[commandLineKey].toStringCollection();

        for(const auto& commandLine : tasks) {
            Task forkedTask = task;
            forkedTask.append(commandLine);
            registerTask(forkedTask, options);
        }
        return true;
    }
} }
