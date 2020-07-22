#ifndef __SHELL_INTERFACE_H__
#define __SHELL_INTERFACE_H__

#include <cstdint>

#include "pathNotFoundError.h"

namespace execHelper::core {

class Task;

/**
 * \brief Represents a shell for executing tasks
 */
class Shell {
  public:
    using ShellReturnCode = uint8_t; //!< brief Shell return code abstraction

    virtual ~Shell() = default;

    /**
     * Executes the given task
     *
     * \param[in] task  The task to execute
     * \returns A shell return code
     * \throws PathNotFoundError    The binary to execute was not found
     */
    virtual auto execute(const Task& task) -> ShellReturnCode = 0;

    /**
     * Checks whether the given shell return code can be associated with a
     * successful execution
     *
     * \param[in] returnCode    The return code to check
     * \returns True    If the return code implies a successful execution
     *          False   Otherwise
     */
    [[nodiscard]] virtual auto
    isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept
        -> bool = 0;

  protected:
    Shell() = default;

    /*! @copydoc config::Argv::Argv(const config::Argv&)
     */
    Shell(const Shell& other) = default;

    /*! @copydoc config::Argv::Argv(config::Argv&&)
     */
    Shell(Shell&& other) = default;

    /*! @copydoc config::Argv::operator=(const config::Argv&)
     */
    auto operator=(const Shell& other) -> Shell& = default;

    /*! @copydoc config::Argv::operator=(config::Argv&&)
     */
    auto operator=(Shell&& other) -> Shell& = default;
};
} // namespace execHelper::core

#endif /* __SHELL_INTERFACE_H__ */
