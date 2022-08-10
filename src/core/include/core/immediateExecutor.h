#ifndef __IMMEDIATE_EXECUTOR_H__
#define __IMMEDIATE_EXECUTOR_H__

#include "executorInterface.h"

#include <functional>
#include <memory>

#include "shell.h"

namespace execHelper::core {
class Task;
/**
 * \brief Implements a straight forward executor that immediately executes the
 * given task. This executor blocks during executes.
 */
class ImmediateExecutor : public ExecutorInterface {
  public:
    using Callback = std::function<void(
        Shell::ShellReturnCode)>; //!< Brief Callback function signature

    /**
     * Create an executor
     *
     * \param[in] shell     The shell to execute the command with
     * \param[in] callback  The function to call with the result when a command
     * finishes
     */
    ImmediateExecutor(Shell& shell, Callback callback) noexcept;

    void execute(const Task& task) noexcept override;

  private:
    const Shell& m_shell;
    Callback m_callback;
};
} // namespace execHelper::core
#endif /* __IMMEDIATE_EXECUTOR_H__ */
