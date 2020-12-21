#ifndef __COMMAND_PLUGIN_H__
#define __COMMAND_PLUGIN_H__

#include "config/variablesMap.h"
#include "executionContext.h"
#include "plugin.h"

namespace execHelper::plugins {
/**
 * \brief Plugin for executing the commands configured on the command line
 */
class CommandPlugin : public Plugin {
  public:
    [[nodiscard]] auto getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const noexcept
        -> config::VariablesMap override;

    [[nodiscard]] auto apply(core::Task task,
                             const config::VariablesMap& variables,
                             const ExecutionContext& context) const
        -> core::Tasks override;

    [[nodiscard]] auto summary() const noexcept -> std::string override;
};
} // namespace execHelper::plugins

#endif /* __COMMAND_PLUGIN_H__ */
