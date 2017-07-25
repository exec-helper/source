#ifndef __EXECUTOR_INTERFACE_H__
#define __EXECUTOR_INTERFACE_H__

namespace execHelper {
    namespace core {
        class Task;
    } // namespace core
} // namespace execHelper

namespace execHelper { namespace core {
    /**
     * \brief Interface for executing tasks
     */
    class ExecutorInterface {
        public:
            virtual ~ExecutorInterface() = default;

            /**
             * Execute the given task
             *
             * \param[in] task  The task to execute
             * \returns True    When the task is successfully added to the executor. Note: this does not necessarily mean that the task got executed or will successfully execute.
             *          False   Otherwise
             */
            virtual void execute(const Task& task) noexcept = 0;

        protected:
            ExecutorInterface() = default;
    };
} // namespace core
} // namespace execHelper

#endif /* __EXECUTOR_INTERFACE_H__ */
