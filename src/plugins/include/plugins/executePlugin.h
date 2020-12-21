#ifndef __EXECUTE_PLUGIN_H__
#define __EXECUTE_PLUGIN_H__

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl>

#include "config/commandLineOptions.h"
#include "config/pattern.h"

#include "config/variablesMap.h"
#include "executionContext.h"
#include "plugin.h"

namespace execHelper::plugins {
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

    [[nodiscard]] auto getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const noexcept
        -> config::VariablesMap override;

    [[nodiscard]] auto apply(core::Task task,
                             const config::VariablesMap& variables,
                             const ExecutionContext& context) const
        -> core::Tasks override;

    [[nodiscard]] auto summary() const noexcept -> std::string override;

    /**
     * Returns a list with the names of the given plugins
     *
     * @param[in] plugins   The list of plugins to get the name for
     * @returns A list of plugin names
     */
    static auto getPluginNames(const Plugins& plugins) noexcept
        -> std::vector<std::string>;

  private:
    static auto getNextStep(const config::Command& command,
                            const config::Command& originalCommand,
                            const config::SettingsNode& settings,
                            const Plugins& plugins) noexcept
        -> std::shared_ptr<const Plugin>;

    static auto
    getVariablesMap(config::VariablesMap* variables,
                    const std::vector<config::SettingsKeys>& keys,
                    const config::SettingsNode& rootSettings) noexcept -> bool;
    static void index(config::VariablesMap* variables,
                      const config::SettingsNode& settings,
                      const config::SettingsKeys& key) noexcept;

    const config::CommandCollection m_commands;
    const config::CommandCollection m_initialCommands;
};
} // namespace execHelper::plugins

#endif /* __EXECUTE_PLUGIN_H__ */
