#ifndef __EXECUTE_PLUGIN_H__
#define __EXECUTE_PLUGIN_H__

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl>

#include "config/commandLineOptions.h"
#include "config/fleetingOptionsInterface.h"
#include "config/pattern.h"
#include "config/patternsHandler.h"
#include "config/settingsNode.h"

#include "plugin.h"

namespace execHelper::plugins {
using Plugins = std::map<std::string, std::shared_ptr<const Plugin>>;

/**
 * \brief Exception thrown when the requested plugin is invalid
 *
 * Exception thrown when the requested plugin is invalid e.g. due to the fact that it can not be found
 */
struct InvalidPlugin : public std::runtime_error {
  public:
    /**
     * Create an invalid plugin
     *
     * \param[in] msg   A message detailing the specifics of the exception
     */
    inline explicit InvalidPlugin(const std::string& msg)
        : std::runtime_error(msg) {}

    /*! @copydoc InvalidPlugin(const std::string&)
     */
    inline explicit InvalidPlugin(const char* msg) : std::runtime_error(msg) {}
};

/**
 * \brief Plugin for executing arbitrary configured commands and/or plugins
 *
 * The ExecutePlugin handles the context of and calls all plugins. It uses the prototype pattern for retrieving a map of plugins it can call.
 */
class ExecutePlugin : public Plugin {
  public:
    /**
     * Create an executePlugin instance
     *
     * \param[in] commandsToExecute  The commands to execute with this plugin
     * instance
     */
    explicit ExecutePlugin(
        const config::CommandCollection& commandsToExecute) noexcept;

    /**
     * \param[in] commandsToExecute The commands to execute
     * \param[in] initialCommand    The initial command that is being executed
     */
    ExecutePlugin(const config::CommandCollection& commandsToExecute,
                  const config::Command& initialCommand) noexcept;

    config::VariablesMap getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const
        noexcept override;
    bool apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept override;

    /**
     * Returns a list with the names of all known plugins
     *
     * @returns A list of plugin names
     */
    static auto getPluginNames() noexcept -> std::vector<std::string>;

    /**
     * Returns an instance of the plugin associated with the given name
     *
     * \param[in] pluginName    The plugin to get the associated instance from
     * \returns A pointer to the new instance
     * \throws  InvalidPlugin   When no plugin associated with the given pluginName is found
     */
    static std::shared_ptr<const Plugin>
    getPlugin(const std::string& pluginName);

    /**
     * Push the given fleeting options on the stack
     *
     * \param[in] fleetingOptions   The fleeting options to use. The last option
     * on the stack will be used for calling the commands. \returns True    if
     * the options were successfully pushed False   otherwise
     */
    static bool push(gsl::not_null<const config::FleetingOptionsInterface*>
                         fleetingOptions) noexcept;

    /**
     * Push the given settings node on the stack
     *
     * \param[in] settings   The settings node to use. The last settings node on
     * the stack will be used for calling the commands. \returns True    if the
     * settings node was successfully pushed False   otherwise
     */
    static bool push(config::SettingsNode&& settings) noexcept;

    /**
     * Push the given patterns on the stack
     *
     * \param[in] patterns   The patterns to use. The last patterns on the stack
     * will be used for calling the commands.
     * \returns True    if the patterns were successfully pushed
     *          False   otherwise
     */
    static bool push(config::Patterns&& patterns) noexcept;

    /**
     * Push the plugin prototypes to the stack
     *
     * \param[in] plugins   Mapping of discovered plugin prototypes
     */
    static void push(Plugins&& plugins) noexcept;

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

    /**
     * Pop the last plugin prototypes from the stack
     */
    static void popPlugins() noexcept;

  private:
    static auto getNextStep(const config::Command& command,
                            const config::Command& originalCommand) noexcept
        -> std::shared_ptr<const Plugin>;
    static bool
    getVariablesMap(config::VariablesMap* variables,
                    const std::vector<config::SettingsKeys>& keys,
                    const config::SettingsNode& rootSettings) noexcept;
    static void index(config::VariablesMap* variables,
                      const config::SettingsNode& settings,
                      const config::SettingsKeys& key) noexcept;

    const config::CommandCollection m_commands;
    const config::CommandCollection m_initialCommands;

    static std::vector<gsl::not_null<const config::FleetingOptionsInterface*>>
        m_fleeting;
    static std::vector<config::SettingsNode> m_settings;
    static std::vector<config::PatternsHandler> m_patterns;
    static std::vector<Plugins> m_plugins;
};
} // namespace execHelper::plugins

#endif /* __EXECUTE_PLUGIN_H__ */
