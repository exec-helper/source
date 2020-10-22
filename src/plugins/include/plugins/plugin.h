#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <functional>
#include <utility>

#include "config/commandLineOptions.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"

namespace execHelper {
namespace config {
class FleetingOptionsInterface;
}
namespace core {
class Task;
} // namespace core
} // namespace execHelper

namespace execHelper {
namespace plugins {
/**
 * \brief Interface declaration to which plugins should comply
 */
class Plugin {
  public:
    Plugin(const Plugin& other) = delete;
    Plugin(Plugin&& other) noexcept = delete;

    virtual ~Plugin() = default;

    Plugin& operator=(const Plugin& other) = delete;
    Plugin& operator=(Plugin&& other) noexcept = delete;

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
    [[nodiscard]] static config::SettingsKey getPatternsKey() noexcept {
        return "patterns";
    }

    /**
     * Apply the plugin
     *
     * \param[in] task      The task to extend
     * \param[in] variables The variables map containing the values to use for
     * the executed command for this specific plugin \param[in] patterns  The
     * patterns that were configured to use for the executed command for this
     * specific plugin
     *
     * \returns True    If the application was successful False
     * Otherwise
     */
    [[nodiscard]] virtual bool
    apply(core::Task task, const config::VariablesMap& variables,
          const config::Patterns& patterns) const noexcept = 0;

    /**
     * Returns a summary of the specific plugin
     *
     * \returns A short description of the plugin
     */
    [[nodiscard]] virtual std::string summary() const noexcept = 0;

  protected:
    Plugin() = default;
};

using ExecuteCallback = std::function<void(const core::Task&)>;

/**
 * Register a callback function. This replaces the previous callback function.
 *
 * \param[in] callback  The function to call when a task is registered.
 */
void registerExecuteCallback(const ExecuteCallback& callback) noexcept;

/**
 * Unregister the current callback function.
 */
void unregisterExecuteCallback() noexcept;

/**
 * Register a finished task to an executor
 *
 * \param[in] task  The finished task
 * \returns True    If the task was successfully registered
 *          False   Otherwise
 */
[[nodiscard]] auto registerTask(const core::Task& task) noexcept -> bool;

/**
 * Output specifics of the plugin
 */
inline auto operator<<(std::ostream& os, // NOLINT(fuchsia-overloaded-operator)
                       const Plugin& plugin) -> std::ostream& {
    os << plugin.summary();
    return os;
}
} // namespace plugins
} // namespace execHelper

#endif /* __PLUGIN_H__ */
