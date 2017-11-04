#ifndef __PLUGIN_H__
#define __PLUGIN_H__

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
    /*! @copydoc config::Argv::Argv(const Argv&)
     */
    Plugin(const Plugin& other) = delete;

    /*! @copydoc config::Argv::Argv(Argv&&)
     */
    Plugin(Plugin&& other) noexcept = delete;
    virtual ~Plugin() = default;

    /*! @copydoc config::Argv::operator=(const Argv&)
     */
    Plugin& operator=(const Plugin& other) = delete;

    /*! @copydoc config::Argv::operator=(Argv&&)
     */
    Plugin& operator=(Plugin&& other) noexcept = delete;

    /**
     * Get the name of the plugin
     *
     * \returns The name of the plugin
     */
    virtual std::string getPluginName() const noexcept = 0;

    /**
     * Returns the default variables map based on the given fleeting options
     *
     * \param[in] fleetingOptions   The fleeting options to base the defaults on
     * \returns The default constructed variables map
     */
    virtual config::VariablesMap getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const
        noexcept = 0;

    /**
     * Returns the root settings key for the patterns of a plugin
     *
     * \returns The root settings key
     */
    virtual config::SettingsKey getPatternsKey() const noexcept {
        return "patterns";
    }

    /**
     * Apply the plugin
     *
     * \param[in] task      The task to extend
     * \param[in] variables The variables map containing the values to use for
     * the executed command for this specific plugin \param[in] patterns  The
     * patterns that were configured to use for the executed command for this
     * specific plugin \returns True    If the application was successful False
     * Otherwise
     */
    virtual bool apply(core::Task task, const config::VariablesMap& variables,
                       const config::Patterns& patterns) const noexcept = 0;

  protected:
    Plugin() = default;
};

using ExecuteCallback = std::function<void(const core::Task&)>;

void registerExecuteCallback(const ExecuteCallback& callback) noexcept;

/**
 * Register a finished task to an executor
 *
 * \param[in] task  The finished task
 * \returns True    If the task was successfully registered
 *          False   Otherwise
 */
bool registerTask(const core::Task& task) noexcept;
} // namespace plugins
} // namespace execHelper

#endif /* __PLUGIN_H__ */
