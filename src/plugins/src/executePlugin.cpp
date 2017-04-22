#include "executePlugin.h"

#include "config/settingsNode.h"
#include "core/options.h"
#include "core/task.h"
#include "log/log.h"

#include "bootstrap.h"
#include "clangStaticAnalyzer.h"
#include "clangTidy.h"
#include "commandLineCommand.h"
#include "cppcheck.h"
#include "lcov.h"
#include "make.h"
#include "memory.h"
#include "plugin.h"
#include "pmd.h"
#include "scons.h"
#include "selector.h"
#include "valgrind.h"

using std::string;
using std::vector;
using std::unique_ptr;
using std::forward;

using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::Options;

namespace {
    // Create this function our selves in order to stay compliant with c++11
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return unique_ptr<T>(new T(forward<Args>(args)...));
    }
}

namespace execHelper { namespace plugins {
    ExecutePlugin::ExecutePlugin(vector<string> commands) noexcept :
        m_commands(std::move(commands))
    {
        ;
    }

    bool ExecutePlugin::apply(const Command& command, Task& task, const Options& options) const noexcept {
        for(const auto& pluginName : m_commands) {
            Task newTask = task;
            unique_ptr<Plugin> plugin = getPlugin(pluginName);
            Command commandToPass = command;
            if(!plugin) {
                // Check if it exists as an other target in the settings
                if(options.getSettings().contains(pluginName)) {
                    // Then use executeplugin as the plugin 
                    plugin = make_unique<ExecutePlugin>(options.getSettings(pluginName).values());
                    commandToPass = pluginName;
                } else {
                    user_feedback_error("Could not find a command or plugin called '" << pluginName << "'");
                    return false;
                }
            }
            if(!plugin || !plugin->apply(commandToPass, newTask, options)) {
                return false;
            }
        }
        return true;
    }

    unique_ptr<Plugin> ExecutePlugin::getPlugin(const string& pluginName) noexcept {
        if(pluginName == "command-line-command") {
            return make_unique<CommandLineCommand>();
        }
        if(pluginName == "scons") {
            return make_unique<Scons>(); 
        }
        if(pluginName == "make") {
            return make_unique<Make>(); 
        }
        if(pluginName == "bootstrap") {
            return make_unique<Bootstrap>(); 
        }
        if(pluginName == "cppcheck") {
            return make_unique<Cppcheck>();
        }
        if(pluginName == "clang-static-analyzer") {
            return make_unique<ClangStaticAnalyzer>();
        }
        if(pluginName == "selector") {
            return make_unique<Selector>();
        }
        if(pluginName == "memory") {
            return make_unique<Memory>();
        }
        if(pluginName == "valgrind") {
            return make_unique<Valgrind>();
        }
        if(pluginName == "pmd") {
            return make_unique<Pmd>();
        }
        if(pluginName == "lcov") {
            return make_unique<Lcov>();
        }
        if(pluginName == "clang-tidy") {
            return make_unique<ClangTidy>();
        }
        return unique_ptr<Plugin>();
    }
} }
