#include <utility>

#include "executionContext.h"
#include "memory.h"

#include <stdexcept>

#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"

using std::move;
using std::string;

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;

using namespace std::string_literals;

namespace execHelper::plugins {
Memory::Memory() noexcept
    : m_apply([](Task task, const VariablesMap& /*variablesMap*/,
                 const ExecutionContext& /*context*/) noexcept -> Tasks {
          return {move(task)};
      }) {
    ;
}

Memory::Memory(ApplyFunction apply) noexcept : m_apply(move(apply)) { ; }

auto Memory::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept
    -> VariablesMap {
    return VariablesMap("memory");
}

auto Memory::apply(Task task, const VariablesMap& variables,
                   const ExecutionContext& context) const -> Tasks {
    return m_apply(task, variables, context);
}

auto Memory::summary() const noexcept -> std::string {
    return "Memory (internal)";
}
} // namespace execHelper::plugins
