#ifndef __COMMAND_LINE_COMMAND_HPP
#define __COMMAND_LINE_COMMAND_HPP

#include "plugin.h"

namespace execHelper::plugins {
/**
 * \brief Plugin for executing arbitrary command lines
 */
class CommandLineCommand : public Plugin {
  public:
    config::VariablesMap
    getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions)
        const noexcept override;
    bool apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept override;
    std::string summary() const noexcept override;
};
} // namespace execHelper::plugins

#endif /* __COMMAND_LINE_COMMAND_HPP */
