#ifndef __MAKE_H__
#define __MAKE_H__

#include "buildPlugin.h"
#include "core/task.h"

namespace execHelper {
namespace plugins {
/**
 * \brief   Plugin for running make
 */
class Make : public BuildPlugin {
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

#endif /* __MAKE_H__ */
