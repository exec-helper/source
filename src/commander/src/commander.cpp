#include "commander.h"

#include <iostream>

#include "config/settingsNode.h"
#include "core/execHelperOptions.h"
#include "plugins/plugin.h"
#include "plugins/commandLineCommand.h"

using std::cout;
using std::endl;
using std::string;
using std::shared_ptr;
using std::make_shared;
using execHelper::config::SettingsNode;
using execHelper::plugins::Plugin;

namespace execHelper { namespace core {
    Commander::Commander(const ExecHelperOptions& options) :
        m_options(options)
    {
        ;
    }

    shared_ptr<Plugin> Commander::getPlugin(const string& pluginName) noexcept {
        if(pluginName == "command-line-command") {
                return make_shared<plugins::CommandLineCommand>();
        }
        return shared_ptr<Plugin>();
    }

    void Commander::executePlugin(const string& pluginName, const string& command, const ExecHelperOptions& options) noexcept {
        Task task;
        shared_ptr<Plugin> plugin = getPlugin(pluginName);
        if(plugin) {
            plugin->apply(command, task, options);
        }
    }

    void Commander::run() noexcept {
        const SettingsNode commandSettings = m_options.getSettings("commands");

        for(const auto& command : m_options.getCommands()) {
            if(! commandSettings.contains(command)) {
                cout << "Error: undefined command" << endl;
            }        
            const SettingsNode commandSpecificSettings = m_options.getSettings(command);
            for(const auto& plugin : commandSpecificSettings.toStringCollection()) {
                executePlugin(plugin, command, m_options);
            }
        }
    }
} }
