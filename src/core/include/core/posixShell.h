#ifndef __POSIX_SHELL_H__
#define __POSIX_SHELL_H__

#include <csignal>

#include "shell.h"
#include "task.h"

namespace execHelper::core {
/**
 * \brief Implementation for Shell that represents a posix shell
 */
class PosixShell final : public Shell {
  public:
    [[nodiscard]] auto execute(const Task& task) const
        -> ShellReturnCode override;

    [[nodiscard]] auto
    isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept
        -> bool override;

  private:
    // cppcheck-suppress unusedPrivateFunction
    //void childProcessExecute(const Task& task) const noexcept;
    // cppcheck-suppress unusedPrivateFunction
    //ShellReturnCode waitForChild(pid_t pid) const noexcept;

    static auto shellExpand(const Task& task) noexcept -> TaskCollection;
    static auto wordExpand(const Task& task) noexcept -> TaskCollection;
};
} // namespace execHelper::core

#endif /* __POSIX_SHELL_H__ */
