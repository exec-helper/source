#include "executePlugin.h"

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/options.h"
#include "core/task.h"

#include "plugins/plugin.h"
#include "plugins/commandLineCommand.h"
#include "plugins/scons.h"
#include "plugins/make.h"
#include "plugins/bootstrap.h"
#include "plugins/cppcheck.h"

using std::string;
using std::shared_ptr;
using std::make_shared;

using execHelper::config::SettingsNode;
using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::Options;

namespace execHelper { namespace plugins {
    ExecutePlugin::ExecutePlugin(const SettingsNode& settings) :
        m_settings(settings)
    {
        ;
    }

    bool ExecutePlugin::apply(const Command& command, Task& task, const Options& options) const noexcept {
        for(const auto& pluginName : m_settings.m_values) {
            Task newTask = task;
            shared_ptr<Plugin> plugin = getPlugin(pluginName.m_key);
            Command commandToPass = command;
            if(!plugin) {
                // Check if it exists as an other target in the settings
                if(options.getSettings(pluginName.m_key) != options.getSettings()) {
                    // Then use executeplugin as the plugin 
                    plugin = make_shared<ExecutePlugin>(options.getSettings(pluginName.m_key));
                    commandToPass = pluginName.m_key;
                } else {
                    user_feedback("Could not find a command or plugin called " << pluginName);
                    return false;
                }
            }
            if(!plugin || !plugin->apply(commandToPass, newTask, options)) {
                return false;
            }
        }
        return true;
    }

    shared_ptr<Plugin> ExecutePlugin::getPlugin(const string& pluginName) noexcept {
        if(pluginName == "command-line-command") {
            return make_shared<CommandLineCommand>();
        } else if(pluginName == "scons") {
            return make_shared<Scons>(); 
        } else if(pluginName == "make") {
            return make_shared<Make>(); 
        } else if(pluginName == "bootstrap") {
            return make_shared<Bootstrap>(); 
        } else if(pluginName == "cppcheck") {
            return make_shared<Cppcheck>();
        }
        return shared_ptr<Plugin>();
    }
} }
