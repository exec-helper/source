#ifndef __EXECUTOR_STUB_H__
#define __EXECUTOR_STUB_H__

#include <vector>

#include "core/executorInterface.h"
#include "core/task.h"

namespace execHelper { namespace core { namespace test {
    class ExecutorStub : public ExecutorInterface {
        public:
            using TaskQueue = std::vector<Task>;
            bool execute(const Task& task) noexcept override {
                m_executedTasks.push_back(task);
                return true;
            }

            const TaskQueue& getExecutedTasks() const noexcept {
                return m_executedTasks;
            }

        private:
            TaskQueue m_executedTasks;
    };
} // namespace test
} // namespace core
} // namespace execHelper

#endif /* __EXECUTOR_STUB_H__ */
