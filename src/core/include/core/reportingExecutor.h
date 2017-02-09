#ifndef __REPORTING_EXECUTOR_H__
#define __REPORTING_EXECUTOR_H__

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
        class ReportingExecutor : public ExecutorInterface {
            public:
                ReportingExecutor() noexcept;

                virtual bool execute(const Task& task) noexcept override;
        };
    }
}
#endif /* __REPORTING_EXECUTOR_H__ */
