#include "commander.h"

#include <iostream>

#include "log/log.h"
#include "core/options.h"
#include "core/task.h"
#include "plugins/commandPlugin.h"

using execHelper::core::Options;
using execHelper::core::Task;
using execHelper::plugins::CommandPlugin;

namespace execHelper { namespace commander {
    Commander::Commander(const Options& options, core::EnvironmentCollection&& env) :
        m_options(options),
        m_env(env)
    {
        ;
    }

    bool Commander::run() noexcept {
        for(const auto& command : m_options.getCommands()) {
            CommandPlugin plugin;
            Task task;
            task.setEnvironment(m_env);
            if(! plugin.apply(command, task, m_options)) {
                return false;
            }
        }
        return true;
    }
} }
