#include "memory.h"

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"

using std::string;

using gsl::czstring;

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

using namespace std::string_literals;

namespace {
const czstring<> PLUGIN_NAME = "memory";
} // namespace

namespace execHelper::plugins {
Memory::Memories               // NOLINT(fuchsia-statically-constructed-objects)
    Memory::m_executions = {}; // NOLINT(readability-redundant-declaration)
bool Memory::m_returnCode = true; // NOLINT(readability-redundant-declaration)

auto Memory::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept
    -> VariablesMap {
    return VariablesMap(PLUGIN_NAME);
}

auto Memory::apply(Task task, const VariablesMap& variables,
                   const Patterns& patterns) const noexcept -> bool {
    Memory_t newElement(task, variables, patterns);
    m_executions.emplace_back(newElement);
    return m_returnCode;
}

auto Memory::summary() const noexcept -> std::string {
    return "Memory (internal)";
}

auto Memory::getExecutions() noexcept -> const Memory::Memories& {
    return m_executions;
}

void Memory::reset() noexcept {
    m_executions.clear();
    m_returnCode = true;
}

void Memory::setReturnCode(bool returnCode) noexcept {
    m_returnCode = returnCode;
}

MemoryHandler::MemoryHandler() {
    // Reset the memory here too, just to be sure
    Memory::reset();
}

MemoryHandler::~MemoryHandler() { Memory::reset(); }

auto MemoryHandler::getExecutions() noexcept -> const Memory::Memories& {
    return Memory::getExecutions();
}

void MemoryHandler::reset() noexcept { Memory::reset(); }

void MemoryHandler::setReturnCode(bool returnCode) noexcept {
    Memory::setReturnCode(returnCode);
}

SpecialMemory::SpecialMemory(bool returnCode, size_t expectedSize) noexcept
    : m_returnCode(returnCode) {
    m_executions.reserve(expectedSize);
}

auto SpecialMemory::getVariablesMap(
    const config::FleetingOptionsInterface& /*fleetingOptions*/) const noexcept
    -> config::VariablesMap {
    return VariablesMap(PLUGIN_NAME);
}

auto SpecialMemory::apply(core::Task task,
                          const config::VariablesMap& variables,
                          const config::Patterns& patterns) const noexcept
    -> bool {
    m_executions.emplace_back(task, variables, patterns);
    return m_returnCode;
}

auto SpecialMemory::summary() const noexcept -> string {
    return "SpecialMemory (internal)";
}

auto SpecialMemory::getExecutions() noexcept -> const Memories& {
    return m_executions;
}

} // namespace execHelper::plugins
