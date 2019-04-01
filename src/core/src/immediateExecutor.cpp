#include "immediateExecutor.h"

#include <iostream>

#include "log/log.h"

#include "shell.h"
#include "task.h"

using std::move;

namespace execHelper::core {

ImmediateExecutor::ImmediateExecutor(std::shared_ptr<Shell> shell,
                                     Callback callback) noexcept
    : m_shell(move(shell)), m_callback(move(callback)) {
    assert(m_shell != nullptr);
}

void ImmediateExecutor::execute(const Task& task) noexcept {
    user_feedback_info("Executing " << task.toString());
    Shell::ShellReturnCode returnCode = m_shell->execute(task);
    if(!m_shell->isExecutedSuccessfully(returnCode)) {
        m_callback(returnCode);
    }
}
} // namespace execHelper::core
