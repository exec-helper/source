#include "posixShell.h"

#include <stdlib.h>

#include "task.h"

namespace execHelper { namespace core {
    PosixShell::ShellReturnCode PosixShell::execute(const Task& task) noexcept {
        return system(task.toString().c_str()); 
    }

    bool PosixShell::isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept {
        return returnCode == 0U;
    }
} }
