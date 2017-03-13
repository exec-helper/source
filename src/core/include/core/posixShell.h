#ifndef __POSIX_SHELL_H__
#define __POSIX_SHELL_H__

#include <signal.h>

#include "shell.h"
#include "task.h"

namespace execHelper {
    namespace core {
        class PosixShell : public Shell {
            public:
                virtual ShellReturnCode execute(const Task& task) noexcept;
                virtual bool isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept;

            private:
                void childProcessExecute(const Task& task) const noexcept;
                ShellReturnCode waitForChild(pid_t pid) const noexcept;

                static TaskCollection shellExpand(const TaskCollection& task) noexcept;
                static TaskCollection wordExpand(const TaskCollection& task) noexcept;
        };
    }
}

#endif  /* __POSIX_SHELL_H__ */
