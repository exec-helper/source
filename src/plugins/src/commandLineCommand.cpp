#include "commandLineCommand.h"

#include "config/settingsNode.h"

using execHelper::core::Task;
using execHelper::core::ExecHelperOptions;
using execHelper::core::TaskCollection;
using execHelper::config::SettingsNode;

namespace execHelper { namespace plugins {
    void CommandLineCommand::apply(const std::string& /*command*/, Task& task, const ExecHelperOptions& options) const noexcept {
        const SettingsNode& settings = options.getSettings({"command-line-command"});  
        TaskCollection tasks = settings["command"].toStringCollection();
        task.append(tasks);
        registerTask(task, options);
    }
} }
