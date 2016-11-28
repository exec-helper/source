#include "commandLineCommand.h"

#include "config/settingsNode.h"

using execHelper::core::Task;
using execHelper::core::ExecHelperOptions;
using execHelper::core::TaskCollection;
using execHelper::config::SettingsNode;

namespace execHelper { namespace plugins {
    bool CommandLineCommand::apply(const std::string& /*command*/, Task& task, const ExecHelperOptions& options) const noexcept {
        const SettingsNode& settings = options.getSettings({"command-line-command"});  
        TaskCollection tasks = settings["command"].toStringCollection();
        for(const auto& command : tasks) {
            Task forkedTask = task;
            forkedTask.append(command);
            registerTask(forkedTask, options);
        }
        return true;
    }
} }
