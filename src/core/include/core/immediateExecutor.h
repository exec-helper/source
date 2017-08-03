#ifndef __IMMEDIATE_EXECUTOR_H__
#define __IMMEDIATE_EXECUTOR_H__

#include "executorInterface.h"

#include <functional>
#include <memory>

#include "shell.h"

namespace execHelper {
    namespace core {
        class Task;
    }
}

namespace execHelper {
    namespace core {
        /**
         * \brief Implements a straight forward executor that immediately executes the given task. This executor blocks during executes.
         */
        class ImmediateExecutor : public ExecutorInterface {
            public:
                using Callback = std::function<void(Shell::ShellReturnCode)>;

                /**
                 * Create an executor
                 *
                 * \param shell     The shell to execute the command with
                 * \param callback  The function to call with the result when a command finishes
                 */
                ImmediateExecutor(std::shared_ptr<Shell> shell, Callback callback) noexcept;

                bool execute(const Task& task) noexcept override;

            private:
                std::shared_ptr<Shell> m_shell;
                Callback m_callback;
        };
    }
}
#endif /* __IMMEDIATE_EXECUTOR_H__ */
