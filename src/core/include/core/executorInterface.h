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
            /*! @copydoc config::Argv::Argv(const Argv&)
             * \note Deleted
             */
            ExecutorInterface(const ExecutorInterface& other) = delete;

            /*! @copydoc config::Argv::Argv(Argv&&)
             * \note Deleted
             */
            ExecutorInterface(ExecutorInterface&& other) = delete;
            virtual ~ExecutorInterface() = default;

            /*! @copydoc config::Argv::operator=(const Argv&)
             * \note Deleted
             */
            ExecutorInterface& operator=(const ExecutorInterface& other) = delete;

            /*! @copydoc config::Argv::operator=(Argv&&)
             * \note Deleted
             */
            ExecutorInterface& operator=(ExecutorInterface&& other) = delete;

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
