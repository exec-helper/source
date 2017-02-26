#ifndef __EXECUTOR_STUB_H__
#define __EXECUTOR_STUB_H__

#include <vector>

#include "core/shell.h"
#include "core/task.h"

namespace execHelper { namespace core { namespace test {
    const Shell::ShellReturnCode RETURN_CODE_SUCCESS = 0U;

    class ShellStub : public Shell {
        public:
            typedef std::vector<Task> TaskQueue;

            ShellStub(ShellReturnCode returnCode = RETURN_CODE_SUCCESS) :
                Shell(),
                m_returnCode(returnCode)
            {
                ;
            }

            virtual ShellReturnCode execute(const Task& task) noexcept {
                m_executedTasks.push_back(task);
                return m_returnCode;
            }

            virtual bool isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept {
                return returnCode == RETURN_CODE_SUCCESS;
            }

            const TaskQueue& getExecutedTasks() const noexcept {
                return m_executedTasks;
            }

        private:
            TaskQueue m_executedTasks;
            ShellReturnCode m_returnCode; 
    };
} } }

#endif /* __EXECUTOR_STUB_H__ */
