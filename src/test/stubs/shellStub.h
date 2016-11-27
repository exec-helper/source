#ifndef __EXECUTOR_STUB_H__
#define __EXECUTOR_STUB_H__

#include <vector>

#include "core/shell.h"
#include "core/task.h"

namespace execHelper { namespace core { namespace test {
    class ShellStub : public Shell {
        public:
            typedef std::vector<Task> TaskQueue;

            ShellStub() :
                Shell(),
                RETURN_CODE_SUCCESS(0U)
            {
                ;
            }

            virtual ShellReturnCode execute(const Task& task) noexcept {
                m_executedTasks.push_back(task);
                return RETURN_CODE_SUCCESS;
            }

            virtual bool isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept {
                return returnCode == RETURN_CODE_SUCCESS;
            }

            const TaskQueue& getExecutedTasks() const noexcept {
                return m_executedTasks;
            }

        private:
            const ShellReturnCode RETURN_CODE_SUCCESS;
            TaskQueue m_executedTasks;
    };
} } }

#endif /* __EXECUTOR_STUB_H__ */
