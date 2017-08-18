#ifndef __COMMANDER_H__
#define __COMMANDER_H__

#include <map>
#include <memory>
#include <string>

#include "config/path.h"
#include "core/task.h"

namespace execHelper {
    namespace core {
        class Options;
    }

    namespace plugins {
        class Plugin;
    }
}

namespace execHelper {
    namespace commander {
        /**
         * \brief Reads the commands to be run and kicks all the plugins related to these commands
         */
        class Commander {
            public:
                /**
                 * Creates a commander
                 *
                 * \param options   The command line options. ! Note: the passed options must exist until all associated commander objects are destroyed.
                 * \param workingDirectory   The working directory for the commander
                 * \param env       The environment to apply the plugins in
                 */
                Commander(const core::Options& options, config::Path workingDirectory, core::EnvironmentCollection&& env = core::EnvironmentCollection());

                /**
                 * Run the commander
                 *
                 * \returns True    If the command was run successfully
                 *          False   Otherwise
                 */
                bool run() noexcept;
                
            private:
                /**
                 * Applies the plugin associated with the given plugin name
                 * 
                 * \param pluginName    The name of the plugin to execute
                 * \param command       The command that is currently being run
                 * \param options       The options associated with the command being run
                 * \returns True    If the plugin was successfully applied
                 *          False   Otherwise
                 */
                bool executePlugin(const std::string& pluginName, const std::string& command, const core::Options& options) noexcept;

                const core::Options& m_options;
                const config::Path m_workingDirectory;
                const core::EnvironmentCollection m_env;
        };
    }
}

#endif  /* __COMMANDER_H__ */
