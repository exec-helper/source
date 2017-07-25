#include "memory.h"

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"

using gsl::czstring;

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
    const czstring<> PLUGIN_NAME = "memory";
} // namespace

namespace execHelper { namespace plugins {
    Memory::Memories Memory::m_executions = {}; // NOLINT(readability-redundant-declaration)
    bool Memory::m_returnCode = true;   // NOLINT(readability-redundant-declaration)

    std::string Memory::getPluginName() const noexcept {
       return PLUGIN_NAME; 
    }

    VariablesMap Memory::getVariablesMap(const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept {
        return VariablesMap(PLUGIN_NAME);
    }

    bool Memory::apply(Task task, const VariablesMap& variables, const Patterns& patterns) const noexcept {
        Memory_t newElement(task, variables, patterns);
        m_executions.emplace_back(newElement);
        return m_returnCode;
    }

    const Memory::Memories& Memory::getExecutions() noexcept {
        return m_executions;
    }

    void Memory::reset() noexcept {
        m_executions.clear();
        m_returnCode = true;
    }

    void Memory::setReturnCode(bool returnCode) noexcept {
        m_returnCode = returnCode;
    }

    MemoryHandler::MemoryHandler()
    {
        // Reset the memory here too, just to be sure
        Memory::reset(); 
    }

    MemoryHandler::~MemoryHandler() {
        Memory::reset(); 
    }

    const Memory::Memories& MemoryHandler::getExecutions() const noexcept {
        return Memory::getExecutions();
    }

    void MemoryHandler::reset() noexcept {
        Memory::reset();
    }

    void MemoryHandler::setReturnCode(bool returnCode) noexcept {
        Memory::setReturnCode(returnCode);
    }
} // namespace plugins
} // namespace execHelper
