#ifndef __POSIX_SHELL_H__
#define __POSIX_SHELL_H__

#include <csignal>

#include "shell.h"
#include "task.h"

namespace execHelper {
    namespace core {
        /**
         * \brief Implementation for Shell that represents a posix shell
         */
        class PosixShell : public Shell {
            public:
                ShellReturnCode execute(const Task& task) noexcept override;
                bool isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept override;

            private:
                void childProcessExecute(const Task& task) const noexcept;
                ShellReturnCode waitForChild(pid_t pid) const noexcept;

                static TaskCollection shellExpand(const Task& task) noexcept;
                static TaskCollection wordExpand(const Task& task) noexcept;
        };
    }
}

#endif  /* __POSIX_SHELL_H__ */
