#include "immediateExecutor.h"

#include <iostream>

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
        std::cout << "Executing " << task.toString() << std::endl;
        return m_shell.isExecutedSuccessfully(m_shell.execute(task));
    }
} }
