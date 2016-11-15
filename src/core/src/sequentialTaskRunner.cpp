#include "sequentialTaskRunner.h"

#include "executorInterface.h"
#include "task.h"

namespace execHelper { namespace core {
    bool SequentialTaskRunner::addTask(const Task& task) noexcept {
        m_queue.push_back(task);
        return true;
    }

    void SequentialTaskRunner::run(ExecutorInterface& executor) noexcept {
        for(auto& task : m_queue) {
            executor.execute(task);
        }
    }
} }
