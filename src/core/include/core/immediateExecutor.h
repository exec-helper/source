#ifndef __IMMEDIATE_EXECUTOR_H__
#define __IMMEDIATE_EXECUTOR_H__

#include "executorInterface.h"

#include <functional>

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
                typedef std::function<void(Shell::ShellReturnCode)> Callback;

                ImmediateExecutor(Shell& shell, Callback& callback) noexcept;

                virtual bool execute(const Task& task) noexcept override;

            private:
                Shell& m_shell;
                Callback& m_callback;
        };
    }
}
#endif /* __IMMEDIATE_EXECUTOR_H__ */
