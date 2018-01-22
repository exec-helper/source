#ifndef PATH_NOT_FOUND_ERROR_INCLUDE
#define PATH_NOT_FOUND_ERROR_INCLUDE

#include <stdexcept>

namespace execHelper {
namespace core {
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

    /*! @copydoc config::Argv::Argv(const config::Argv&)
     */
    PathNotFoundError(const PathNotFoundError& other) = default;

    /*! @copydoc config::Argv::Argv(config::Argv&&)
     */
    PathNotFoundError(PathNotFoundError&& other) = default;

    ~PathNotFoundError() noexcept override = default;

    /*! @copydoc config::Argv::operator=(const config::Argv&)
     */
    PathNotFoundError& operator=(const PathNotFoundError& other) = default;

    /*! @copydoc config::Argv::operator=(config::Argv&&)
     */
    PathNotFoundError& operator=(PathNotFoundError&& other) = default;
};
} // namespace core
} // namespace execHelper

#endif /* PATH_NOT_FOUND_ERROR_INCLUDE */
