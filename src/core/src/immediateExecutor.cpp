#include "immediateExecutor.h"

#include <iostream>

#include "log/log.h"

#include "pathNotFoundError.h"
#include "shell.h"
#include "task.h"

using std::move;

namespace execHelper::core {

ImmediateExecutor::ImmediateExecutor(
    const gsl::not_null<std::shared_ptr<Shell>>& shell,
    Callback callback) noexcept
    : m_shell(shell), m_callback(move(callback)) {
    ;
}

void ImmediateExecutor::execute(const Task& task) noexcept {
    user_feedback_info("Executing '" << task.toString() << "'");
    try {
        auto returnCode = m_shell->execute(task);
        if(!m_shell->isExecutedSuccessfully(returnCode)) {
            m_callback(returnCode);
        }
    } catch(const PathNotFoundError& e) {
        user_feedback_error("Execution error: " << e.what());
        m_callback(1U);
    }
}
} // namespace execHelper::core
