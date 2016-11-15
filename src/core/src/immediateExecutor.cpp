#include "immediateExecutor.h"

#include "shell.h"
#include "task.h"

namespace execHelper { namespace core {
    ImmediateExecutor::ImmediateExecutor(Shell& shell) noexcept :
        ExecutorInterface(),
        m_shell(shell)    
    {
        ;
    }

    bool ImmediateExecutor::execute(const Task& task) noexcept {
        return m_shell.isExecutedSuccessfully(m_shell.execute(task));
    }
} }
