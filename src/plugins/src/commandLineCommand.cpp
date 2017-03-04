#include "commandLineCommand.h"

#include <string>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"

#include "pluginUtils.h"
#include "configValue.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;

namespace execHelper { namespace plugins {
    bool CommandLineCommand::apply(const std::string& command, Task& task, const Options& options) const noexcept {
        static string commandLineCommandKey("command-line-command");
        static string commandLineKey("command-line");

        const SettingsNode& rootSettings = options.getSettings(commandLineCommandKey);
        boost::optional<TaskCollection> commandLineValues = ConfigValue<TaskCollection>::getSetting(getCommandLineKey(), rootSettings, {command});
        if(commandLineValues == boost::none) {
            user_feedback_error("Could not find the '" << getCommandLineKey() << "' setting for command '" << command << "' in the '" << commandLineCommandKey << "' settings");
            return false;
        }
        if(commandLineValues.get().empty()) {
            user_feedback_error("The '" << getCommandLineKey() << "' setting for command '" << command << "' in the '" << commandLineCommandKey << "' settings is empty");
            return false;
        }
        task.append(commandLineValues.get());

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task commandLineTask = replacePatternCombinations(task, combination);
            registerTask(commandLineTask, options);
        }
        return true;
    }
} }
