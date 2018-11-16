#ifndef NINJA_INCLUDE
#define NINJA_INCLUDE

#include "buildPlugin.h"
#include "core/task.h"

namespace execHelper {
namespace plugins {
/**
 * \brief   Plugin for running ninja
 */
class Ninja : public BuildPlugin {
  public:
    std::string getPluginName() const noexcept override;
    config::VariablesMap getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const
        noexcept override;
    bool apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept override;
};
} // namespace plugins
} // namespace execHelper

#endif /* NINJA_INCLUDE */
