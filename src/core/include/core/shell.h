#ifndef __SHELL_INTERFACE_H__
#define __SHELL_INTERFACE_H__

#include <cstdint>

namespace execHelper {
    namespace core {
        class Task;
    }
}

namespace execHelper {
    namespace core {
        /**
         * \brief Represents a shell for executing tasks
         */
        class Shell {
            public:
                using ShellReturnCode = uint32_t; //!< brief Shell return code abstraction

                /**
                 * Executes the given task
                 *
                 * \param task  The task to execute
                 * \returns A shell return code
                 */
                virtual ShellReturnCode execute(const Task& task) noexcept = 0;

                /**
                 * Checks whether the given shell return code can be associated with a successful execution
                 *
                 * \param returnCode    The return code to check
                 * \returns True    If the return code implies a successful execution
                 *          False   Otherwise
                 */
                virtual bool isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept = 0;

            protected:
                Shell() = default;
        };
    }
}

#endif /* __SHELL_INTERFACE_H__ */
