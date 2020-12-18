#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <functional>
#include <utility>
#include <vector>

#include "config/commandLineOptions.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"

namespace execHelper::config {
class FleetingOptionsInterface;
} // namespace execHelper::config

namespace execHelper::plugins {
/**
 * \brief Interface declaration to which plugins should comply
 */
class Plugin {
  public:
    /**
     * Copy constructor
     *
     * \param[in] other The object to copy from
     */
    Plugin(const Plugin& other) = delete;

    /**
     * Move constructor
     *
     * \param[in] other The object to move from
     */
    Plugin(Plugin&& other) = delete;

    virtual ~Plugin() = default;

    /**
     * Copy assignment operator
     *
     * \param[in] other The object to assign from
     * \returns A reference to this object
     */
    auto operator=(const Plugin& other) -> Plugin& = delete;

    /**
     * Move assignment operator
     *
     * \param[in] other the object to move assign from
     * \returns a reference to this object
     */
    auto operator=(Plugin&& other) -> Plugin& = delete;

    /**
     * Returns the default variables map based on the given fleeting options
     *
     * \param[in] fleetingOptions   The fleeting options to base the defaults on
     * \returns The default constructed variables map
     */
    [[nodiscard]] virtual config::VariablesMap
    getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions)
        const noexcept = 0;

    /**
     * Returns the root settings key for the patterns of a plugin
     *
     * \returns The root settings key
     */
    [[nodiscard]] static auto getPatternsKey() noexcept -> config::SettingsKey {
        return "patterns";
    }

    /**
     * Apply the plugin
     *
     * \param[in] task      The task to extend
     * \param[in] variables The variables map containing the values to use for
     * the executed command for this specific plugin
     *
     * \returns True    If the application was successful False
     * Otherwise
     */
    [[nodiscard]] virtual auto
    apply(core::Task task, const config::VariablesMap& variables) const
        -> core::Tasks = 0;

    /**
     * Returns a summary of the specific plugin
     *
     * \returns A short description of the plugin
     */
    [[nodiscard]] virtual auto summary() const noexcept -> std::string = 0;

  protected:
    Plugin() = default;
};

/**
 * Output specifics of the plugin
 */
inline auto operator<<(std::ostream& os, // NOLINT(fuchsia-overloaded-operator)
                       const Plugin& plugin) -> std::ostream& {
    os << plugin.summary();
    return os;
}
} // namespace execHelper::plugins

#endif /* __PLUGIN_H__ */
