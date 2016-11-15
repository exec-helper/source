#ifndef __TASK_RUNNER_INTERFACE_H__
#define __TASK_RUNNER_INTERFACE_H__

#include <deque>

namespace execHelper {
    namespace core {
        class Task;
        class ExecutorInterface;
    }
}

namespace execHelper {
    namespace core {
        typedef std::deque<Task> TaskQueue;

        class TaskRunnerInterface {
            public:

                virtual bool addTask(const Task& task) = 0;
                virtual void run(ExecutorInterface& executor) = 0;

            protected:
                TaskQueue m_queue;
        };
    }
}

#endif /* __TASK_RUNNER_INTERFACE_H__ */
