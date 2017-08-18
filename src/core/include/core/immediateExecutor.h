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
        class ImmediateExecutor : public ExecutorInterface {
            public:
                using Callback = std::function<void(Shell::ShellReturnCode)>;

                ImmediateExecutor(std::shared_ptr<Shell> shell, Callback callback) noexcept;

                bool execute(const Task& task) noexcept override;

            private:
                std::shared_ptr<Shell> m_shell;
                Callback m_callback;
        };
    }
}
#endif /* __IMMEDIATE_EXECUTOR_H__ */
