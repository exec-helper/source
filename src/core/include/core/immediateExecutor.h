#ifndef __IMMEDIATE_EXECUTOR_H__
#define __IMMEDIATE_EXECUTOR_H__

#include "executorInterface.h"

namespace execHelper {
    namespace core {
        class Shell;
        class Task;
    }
}

namespace execHelper {
    namespace core {
        class ImmediateExecutor : public ExecutorInterface {
            public:
                ImmediateExecutor(Shell& shell) noexcept;

                virtual bool execute(const Task& task) noexcept;

            private:
                Shell& m_shell;
        };
    }
}
#endif /* __IMMEDIATE_EXECUTOR_H__ */
