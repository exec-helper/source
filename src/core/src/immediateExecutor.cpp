#include "immediateExecutor.h"

#include <iostream>

#include "log/log.h"

#include "shell.h"
#include "task.h"

namespace execHelper { namespace core {
    ImmediateExecutor::ImmediateExecutor(Shell& shell, Callback& callback) noexcept :
        m_shell(shell),
        m_callback(callback)
    {
        ;
    }

    bool ImmediateExecutor::execute(const Task& task) noexcept {
        user_feedback_info("Executing " << task.toString());
        Shell::ShellReturnCode returnCode = m_shell.execute(task);
        if(! m_shell.isExecutedSuccessfully(returnCode)) {
            m_callback(returnCode);
            return false;
        }
        return true;

    }
} }
