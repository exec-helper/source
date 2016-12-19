#include "commandPlugin.h"

#include "log/log.h"
#include "config/settingsNode.h"

#include "pluginUtils.h"
#include "executePlugin.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::Options;

namespace execHelper { namespace plugins {
    bool CommandPlugin::apply(const Command& command, Task& task, const Options& options) const noexcept {
        static const string commandKey("commands");
        const SettingsNode rootSettings = options.getSettings(commandKey);
        if(! rootSettings.contains(command)) {
            user_feedback("Error: undefined command");
            return false;
        }        
        ExecutePlugin executePlugin({command});
        return executePlugin.apply(command, task, options);
    }
} }
