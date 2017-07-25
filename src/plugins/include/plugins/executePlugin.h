#ifndef __EXECUTE_PLUGIN_H__
#define __EXECUTE_PLUGIN_H__

#include <memory>
#include <string>
#include <vector>

#include <gsl/gsl>

#include "config/commandLineOptions.h"
#include "config/fleetingOptionsInterface.h"
#include "config/pattern.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"

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
                 * \param[in] commandsToExecute  The commands to execute with this plugin instance
                 */
                explicit ExecutePlugin(const config::CommandCollection& commandsToExecute) noexcept;

                /**
                 * \param[in] commandsToExecute The commands to execute
                 * \param[in] initialCommand    The initial command that is being executed
                 */
                ExecutePlugin(const config::CommandCollection& commandsToExecute, const config::Command& initialCommand) noexcept;

                std::string getPluginName() const noexcept override;
                config::VariablesMap getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions) const noexcept override;
                config::SettingsKey getPatternsKey() const noexcept override;
                bool apply(core::Task task, const config::VariablesMap& variables, const config::Patterns& patterns) const noexcept override;

                /**
                 * Returns an instance of the plugin associated with the given name
                 *
                 * \param[in] pluginName    The plugin to get the associated instance from
                 * \returns A pointer to the new instance
                 */
                static std::unique_ptr<Plugin> getPlugin(const std::string& pluginName) noexcept;

                /**
                 * Push the given fleeting options on the stack
                 *
                 * \param[in] fleetingOptions   The fleeting options to use. The last option on the stack will be used for calling the commands.
                 * \returns True    if the options were successfully pushed
                 *          False   otherwise
                 */
                static bool push(gsl::not_null<const config::FleetingOptionsInterface*> fleetingOptions) noexcept;

                /**
                 * Push the given settings node on the stack
                 *
                 * \param[in] settings   The settings node to use. The last settings node on the stack will be used for calling the commands.
                 * \returns True    if the settings node was successfully pushed
                 *          False   otherwise
                 */
                static bool push(config::SettingsNode&& settings) noexcept;

                /**
                 * Push the given patterns on the stack
                 *
                 * \param[in] patterns   The patterns to use. The last patterns on the stack will be used for calling the commands.
                 * \returns True    if the patters were successfully pushed
                 *          False   otherwise
                 */
                static bool push(config::Patterns&& patterns) noexcept;

                /**
                 * Pop the last fleeting options from the stack
                 */
                static void popFleetingOptions() noexcept;

                /**
                 * Pop the last settings node from the stack
                 */
                static void popSettingsNode() noexcept;

                /**
                 * Pop the last patterns from the stack
                 */
                static void popPatterns() noexcept;

            private:
                static std::unique_ptr<Plugin> getNextStep(const config::Command& command, const config::Command& originalCommand) noexcept;
                static bool getVariablesMap(config::VariablesMap* variables, const std::vector<config::SettingsKeys>& keys, const config::SettingsNode& rootSettings) noexcept;
                static void index(config::VariablesMap* variables, const config::SettingsNode& settings, const config::SettingsKeys& key) noexcept;

                const config::CommandCollection m_commands;
                const config::CommandCollection m_initialCommands;

                static std::vector<gsl::not_null<const config::FleetingOptionsInterface*>> m_fleeting;
                static std::vector<config::SettingsNode> m_settings;
                static std::vector<config::PatternsHandler> m_patterns;
        };
    }
}

#endif  /* __EXECUTE_PLUGIN_H__ */
