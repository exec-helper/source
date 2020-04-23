#ifndef __EXECUTOR_STUB_H__
#define __EXECUTOR_STUB_H__

#include <vector>

#include "core/shell.h"
#include "core/task.h"

namespace execHelper {
namespace core {
namespace test {
const Shell::ShellReturnCode RETURN_CODE_SUCCESS = 0U;

class ShellStub final : public Shell {
  public:
    using TaskQueue = std::vector<Task>;

    explicit ShellStub(ShellReturnCode returnCode = RETURN_CODE_SUCCESS)
        : m_returnCode(returnCode) {
        ;
    }

    ShellReturnCode execute(const Task& task) noexcept override {
        m_executedTasks.push_back(task);
        return m_returnCode;
    }

    bool
    isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept override {
        return returnCode == RETURN_CODE_SUCCESS;
    }

    const TaskQueue& getExecutedTasks() const noexcept {
        return m_executedTasks;
    }

  private:
    TaskQueue m_executedTasks;
    ShellReturnCode m_returnCode;
};
} // namespace test
} // namespace core
} // namespace execHelper

#endif /* __EXECUTOR_STUB_H__ */
