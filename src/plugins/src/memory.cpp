#include <utility>

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
    : m_apply([](Task task,
                 const VariablesMap& /*variablesMap*/) noexcept -> Tasks {
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

auto Memory::apply(Task task, const VariablesMap& variables) const -> Tasks {
    return m_apply(task, variables);
}

auto Memory::summary() const noexcept -> std::string {
    return "Memory (internal)";
}
} // namespace execHelper::plugins
