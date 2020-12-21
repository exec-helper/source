#ifndef __COMMAND_LINE_COMMAND_HPP
#define __COMMAND_LINE_COMMAND_HPP

#include "config/variablesMap.h"
#include "executionContext.h"
#include "plugin.h"

namespace execHelper::plugins {
/**
 * \brief Plugin for executing arbitrary command lines
 */
class CommandLineCommand : public Plugin {
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

#endif /* __COMMAND_LINE_COMMAND_HPP */
