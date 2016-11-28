#include "commander.h"

#include <iostream>

#include "config/settingsNode.h"
#include "core/execHelperOptions.h"
#include "core/task.h"
#include "plugins/plugin.h"
#include "plugins/commandLineCommand.h"
#include "plugins/scons.h"

using std::cout;
using std::endl;
using std::string;
using std::shared_ptr;
using std::make_shared;

using execHelper::core::ExecHelperOptions;
using execHelper::core::Task;
using execHelper::config::SettingsNode;
using execHelper::plugins::Plugin;

namespace execHelper { namespace commander {
    Commander::Commander(const ExecHelperOptions& options) :
        m_options(options)
    {
        ;
    }

    shared_ptr<Plugin> Commander::getPlugin(const string& pluginName) noexcept {
        if(pluginName == "command-line-command") {
            return make_shared<plugins::CommandLineCommand>();
        } else if(pluginName == "scons") {
            return make_shared<plugins::Scons>(); 
        }
        std::cout << "'" << pluginName << "' not registered" << std::endl;
        return shared_ptr<Plugin>();
    }

    bool Commander::executePlugin(const string& pluginName, const string& command, const ExecHelperOptions& options) noexcept {
        Task task;
        shared_ptr<Plugin> plugin = getPlugin(pluginName);
        if(plugin) {
            plugin->apply(command, task, options);
        }
        return false;
    }

    bool Commander::run() noexcept {
        const SettingsNode commandSettings = m_options.getSettings("commands");

        for(const auto& command : m_options.getCommands()) {
            if(! commandSettings.contains(command)) {
                cout << "Error: undefined command" << endl;
                return false;
            }        
            const SettingsNode commandSpecificSettings = m_options.getSettings(command);
            for(const auto& plugin : commandSpecificSettings.toStringCollection()) {
                if(! executePlugin(plugin, command, m_options)) {
                    return false;
                }
            }
        }
        return true;
    }
} }
