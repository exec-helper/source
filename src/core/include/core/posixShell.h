#ifndef __POSIX_SHELL_H__
#define __POSIX_SHELL_H__

#include "shell.h"

namespace execHelper {
    namespace core {
        class Task;
    }
}

namespace execHelper {
    namespace core {
        class PosixShell : public Shell {
            public:
                virtual ShellReturnCode execute(const Task& task) noexcept;
                virtual bool isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept;
        };
    }
}

#endif  /* __POSIX_SHELL_H__ */
