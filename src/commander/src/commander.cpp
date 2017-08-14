#include "commander.h"

#include <iostream>
#include <utility>

#include "core/options.h"
#include "core/task.h"
#include "log/log.h"
#include "plugins/commandPlugin.h"

using std::move;

using execHelper::config::Path;
using execHelper::core::EnvironmentCollection;
using execHelper::core::Options;
using execHelper::core::Task;
using execHelper::plugins::CommandPlugin;

namespace execHelper { namespace commander {
    Commander::Commander(const Options& options, config::Path workingDirectory, core::EnvironmentCollection&& env) :
        m_options(options),
        m_workingDirectory(move(workingDirectory)),
        m_env(env)
    {
        ;
    }

    bool Commander::run() noexcept {
        for(const auto& command : m_options.getCommands()) {
            CommandPlugin plugin;
            Task task({}, m_workingDirectory);
            task.setEnvironment(m_env);
            if(! plugin.apply(command, task, m_options)) {
                return false;
            }
        }
        return true;
    }
} }
