#include "memory.h"

using std::make_tuple;

namespace execHelper { namespace plugins {
    Memory::Memories Memory::m_executions;
    bool Memory::m_returnCode = true;

    bool Memory::apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept {
        Memory_t newElement(command, &task, task, &options);
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
} }
