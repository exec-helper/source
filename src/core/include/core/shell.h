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

    /**
     * Copy constructor
     *
     * \param[in] other The object to copy from
     */
    Shell(const Shell& other) = default;

    /**
     * Move constructor
     *
     * \param[in] other The object to move from
     */
    Shell(Shell&& other) = default;

    /**
     * Copy assignment operator
     *
     * \param[in] other The object to assign from
     * \returns A reference to this object
     */
    auto operator=(const Shell& other) -> Shell& = default;

    /**
     * Move assignment operator
     *
     * \param[in] other the object to move assign from
     * \returns a reference to this object
     */
    auto operator=(Shell&& other) -> Shell& = default;
};
} // namespace execHelper::core

#endif /* __SHELL_INTERFACE_H__ */
