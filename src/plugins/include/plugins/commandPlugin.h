#ifndef __COMMAND_PLUGIN_H__
#define __COMMAND_PLUGIN_H__

#include "plugin.h"

namespace execHelper {
namespace plugins {
/**
 * \brief Plugin for executing the commands configured on the command line
 */
class CommandPlugin : public Plugin {
  public:
    config::VariablesMap getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const
        noexcept override;
    bool apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept override;
};
} // namespace plugins
} // namespace execHelper

#endif /* __COMMAND_PLUGIN_H__ */
