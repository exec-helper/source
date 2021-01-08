#ifndef __COMMAND_LINE_COMMAND_HPP
#define __COMMAND_LINE_COMMAND_HPP

#include <string_view>

#include "config/variablesMap.h"
#include "executionContext.h"
#include "plugin.h"

namespace execHelper::plugins {
[[nodiscard]] auto commandLineCommand(core::Task task,
                                      const config::VariablesMap& variables,
                                      const ExecutionContext& context)
    -> core::Tasks;

[[nodiscard]] inline constexpr auto commandLineCommandSummary() noexcept {
    using namespace std::literals;
    return "Command-line-command (internal)"sv;
}
} // namespace execHelper::plugins

#endif /* __COMMAND_LINE_COMMAND_HPP */
