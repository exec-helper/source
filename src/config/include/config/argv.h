#ifndef __ARGV_H__
#define __ARGV_H__

#include <cstddef>
#include <string>
#include <vector>

namespace execHelper::config {
/**
 * \brief   Wraps the default arguments when calling other programs
 */
class Argv {
  public:
    /**
     * Create an Argv object using the default argc and argv input argument
     * combo
     *
     * \param[in] argc  The number of entries present in the argv argument
     * \param[in] argv  An array of input arguments. Each array element is
     * considered a separate argument.
     */
    Argv(int argc, const char* const* argv) noexcept;

    /**
     * Convert task to the argv/argc structure
     *
     * \param[in] task  The task to take the arguments from
     */
    explicit Argv(const std::vector<std::string>& task) noexcept;

    /**
     * Copy constructor
     *
     * \param[in] other The other object to copy from
     */
    Argv(const Argv& other) noexcept;

    /**
     * Move constructor
     *
     * \param[in] other The other object to copy from
     */
    Argv(Argv&& other) noexcept;
    ~Argv() noexcept;

    /**
     * Copy assignment operator
     *
     * \param[in] other The other object to assign from
     * \returns A reference to this object
     */
    auto operator=(const Argv& other) noexcept -> Argv&;

    /**
     * Move assignment operator
     *
     * \param[in] other The other object to assign from
     * \returns A reference to this object
     */
    auto operator=(Argv&& other) noexcept -> Argv&;

    /**
     * Equality operator
     *
     * \param[in] other The other object to compare with
     * \returns True    If the objects are considered equal
     *          False   Otherwise
     */
    auto operator==(const Argv& other) const noexcept -> bool;

    /**
     * Inequality operator
     *
     * \param[in] other The other object to compare with
     * \returns !operator==(other)
     */
    auto operator!=(const Argv& other) const noexcept -> bool;

    /**
     * Access operator
     *
     * \param[in] index The index of the element to access
     * \returns A C-style string
     */
    auto operator[](size_t index) const noexcept -> char*;

    /**
     * Swap contents
     *
     * \param[in] other The other object to swap with
     */
    void swap(Argv& other) noexcept;

    /**
     * Clear all content
     */
    void clear() noexcept;

    /**
     * Getter for the argc argument
     *
     * \returns The total number of arguments
     */

    [[nodiscard]] auto getArgc() const noexcept -> size_t;

    /**
     * Getter for the argv argument
     *
     * \returns An array of pointers to C-style strings of length \ref getArgc()
     */
    [[nodiscard]] auto getArgv() noexcept -> char**;

    /*! @copydoc getArgv()
     */
    [[nodiscard]] auto getArgv() const noexcept -> const char* const*;

  private:
    using Argv_t = std::vector<char*>;

    /**
     * Create a deep copy
     *
     * \param[in] other The other object to copy
     */
    void deepCopy(const Argv& other) noexcept;

    Argv_t m_argv;
};

/**
 * Streaming operator
 *
 * \param[in] os    The stream to stream to
 * \param[in] argv  The object to stream
 * \returns The stream that was streamed to
 */
auto operator<<(std::ostream& os, const Argv& argv) noexcept -> std::ostream&;
} // namespace execHelper::config

#endif /* __ARGV_H__ */
