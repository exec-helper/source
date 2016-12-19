#include "commander.h"

#include <iostream>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/execHelperOptions.h"
#include "core/task.h"
#include "plugins/commandPlugin.h"

using std::cout;
using std::endl;
using std::string;
using std::shared_ptr;

using execHelper::core::Options;
using execHelper::core::Task;
using execHelper::config::SettingsNode;
using execHelper::plugins::CommandPlugin;

namespace execHelper { namespace commander {
    Commander::Commander(const Options& options) :
        m_options(options)
    {
        ;
    }

    bool Commander::run() noexcept {
        const SettingsNode commandSettings = m_options.getSettings("commands");

        for(const auto& command : m_options.getCommands()) {
            CommandPlugin plugin;
            Task task;
            if(! plugin.apply(command, task, m_options)) {
                return false;
            }
        }
        return true;
    }
} }
