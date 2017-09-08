#ifndef __EXECUTE_PLUGIN_H__
#define __EXECUTE_PLUGIN_H__

#include <memory>
#include <string>
#include <vector>

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        /**
         * \brief Plugin for executing arbitrary configured commands and/or plugins
         */
        class ExecutePlugin : public Plugin {
            public:
                /**
                 * Create an executePlugin instance
                 *
                 * \param[in] commands  The commands to execute with this plugin instance
                 */
                explicit ExecutePlugin(std::vector<std::string> commands) noexcept;
                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;

                /**
                 * Returns an instance of the plugin associated with the given name
                 *
                 * \param[in] pluginName    The plugin to get the associated instance from
                 * \returns A pointer to the new instance
                 */
                static std::unique_ptr<Plugin> getPlugin(const std::string& pluginName) noexcept;

            private:
                const std::vector<std::string> m_commands;
        };
    }
}

#endif  /* __EXECUTE_PLUGIN_H__ */
