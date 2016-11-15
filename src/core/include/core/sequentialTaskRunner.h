#ifndef __SEQUENTIAL_TASK_RUNNER_H__
#define __SEQUENTIAL_TASK_RUNNER_H__

#include "taskRunnerInterface.h"

namespace execHelper {
    namespace core {
        class ExecutorInterface;
        class Task;
    }
}

namespace execHelper {
    namespace core {
        class SequentialTaskRunner : public TaskRunnerInterface {
            public:
                virtual bool addTask(const Task& task) noexcept;
                virtual void run(ExecutorInterface& executor) noexcept;
        };
    }
}



#endif /* __SEQUENTIAL_TASK_RUNNER_H__ */
