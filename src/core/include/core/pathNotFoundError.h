#ifndef PATH_NOT_FOUND_ERROR_INCLUDE
#define PATH_NOT_FOUND_ERROR_INCLUDE

#include <stdexcept>

namespace execHelper::core {
/**
 * \brief Thrown when a path is not found on the system
 */
class PathNotFoundError : public std::runtime_error {
  public:
    /**
     * Create an exception with the given message
     *
     * \param[in] msg   A diagnostic error message
     */
    explicit PathNotFoundError(const std::string& msg)
        : std::runtime_error(msg) {
        ;
    }

    /**
     * Copy constructor
     *
     * \param[in] other The object to copy from
     */
    PathNotFoundError(const PathNotFoundError& other) = default;

    /**
     * Move constructor
     *
     * \param[in] other The object to move from
     */
    PathNotFoundError(PathNotFoundError&& other) = default;

    ~PathNotFoundError() override = default;

    /**
     * Copy assignment operator
     *
     * \param[in] other The object to assign from
     * \returns A reference to this object
     */
    auto operator=(const PathNotFoundError& other)
        -> PathNotFoundError& = default;

    /**
     * Move assignment operator
     *
     * \param[in] other the object to move assign from
     * \returns a reference to this object
     */
    auto operator=(PathNotFoundError&& other) -> PathNotFoundError& = default;
};
} // namespace execHelper::core

#endif /* PATH_NOT_FOUND_ERROR_INCLUDE */
