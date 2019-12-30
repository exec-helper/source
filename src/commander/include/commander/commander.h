#ifndef __COMMANDER_H__
#define __COMMANDER_H__

#include <map>
#include <memory>
#include <string>

#include "config/environment.h"
#include "config/path.h"
#include "config/pattern.h"
#include "core/task.h"

namespace execHelper {
namespace config {
class FleetingOptionsInterface;
class SettingsNode;
} // namespace config

namespace core {
class Options;
}

namespace plugins {
class Plugin;
using Plugins = std::map<std::string, std::shared_ptr<const Plugin>>;
} // namespace plugins
} // namespace execHelper

namespace execHelper {
namespace commander {
/**
 * \brief Reads the commands to be run and kicks all the plugins related to
 * these commands
 */
class Commander {
  public:
    /**
     * Creates a commander
     *
     * \param[in] fleetingOptions    The fleeting options
     * \param[in] settings           The settings node context to use
     * \param[in] patterns           The patterns context to use
     * \param[in] workingDirectory   The working directory for the commander
     * \param[in] env       The environment to apply the plugins in
     * \param[in] plugins   A map of plugin prototypes where each key is associated with a certain plugin
     * \returns True    If the command was run successfully
     *          False   Otherwise
     */
    auto run(const config::FleetingOptionsInterface& fleetingOptions,
             config::SettingsNode settings, config::Patterns patterns,
             const config::Path& workingDirectory,
             const config::EnvironmentCollection& env,
             plugins::Plugins&& plugins) noexcept -> bool;
};
} // namespace commander
} // namespace execHelper

#endif /* __COMMANDER_H__ */
