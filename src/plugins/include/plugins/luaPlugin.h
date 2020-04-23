#ifndef LUA_PLUGIN_INCLUDE
#define LUA_PLUGIN_INCLUDE

#include "config/commandLineOptions.h"
#include "config/path.h"

#include "plugin.h"

namespace execHelper::plugins {
/**
 * \brief Plugin for running a lua plugin as an exec-helper plugin
 */
class LuaPlugin : public Plugin {
  public:
    /**
     * Create a lua plugin for the given script
     *
     * @param[in] script    Absolute path to the lua script to run
     */
    explicit LuaPlugin(config::Path script) noexcept;

    config::VariablesMap
    getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions)
        const noexcept override;
    bool apply(core::Task task, const config::VariablesMap& config,
               const config::Patterns& patterns) const noexcept override;

    std::string summary() const noexcept override;

  private:
    config::Path m_script; //!< Path to the lua script
};
} // namespace execHelper::plugins

#endif /* LUA_PLUGIN_INCLUDE */
