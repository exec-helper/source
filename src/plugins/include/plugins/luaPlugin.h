#ifndef LUA_PLUGIN_INCLUDE
#define LUA_PLUGIN_INCLUDE

#include "config/commandLineOptions.h"
#include "config/path.h"

#include "config/variablesMap.h"
#include "executionContext.h"
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

    [[nodiscard]] auto getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const noexcept
        -> config::VariablesMap override;

    [[nodiscard]] auto apply(core::Task task,
                             const config::VariablesMap& config,
                             const ExecutionContext& context) const
        -> core::Tasks override;

    [[nodiscard]] auto summary() const noexcept -> std::string override;

  private:
    config::Path m_script; //!< Path to the lua script
};
} // namespace execHelper::plugins

#endif /* LUA_PLUGIN_INCLUDE */
