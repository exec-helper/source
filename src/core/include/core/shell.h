#ifndef __SHELL_INTERFACE_H__
#define __SHELL_INTERFACE_H__

#include <cstdint>

#include "pathNotFoundError.h"

namespace execHelper {
namespace core {
class Task;
} // namespace core
} // namespace execHelper

namespace execHelper {
namespace core {
/**
 * \brief Represents a shell for executing tasks
 */
class Shell {
  public:
    using ShellReturnCode = uint8_t; //!< brief Shell return code abstraction

    /**
     * Executes the given task
     *
     * \param[in] task  The task to execute
     * \returns A shell return code
     * \throws PathNotFoundError    The binary to execute was not found
     */
    virtual ShellReturnCode execute(const Task& task) = 0;

    /**
     * Checks whether the given shell return code can be associated with a
     * successful execution
     *
     * \param[in] returnCode    The return code to check
     * \returns True    If the return code implies a successful execution
     *          False   Otherwise
     */
    virtual bool isExecutedSuccessfully(ShellReturnCode returnCode) const
        noexcept = 0;

  protected:
    Shell() = default;
};
} // namespace core
} // namespace execHelper

#endif /* __SHELL_INTERFACE_H__ */
