#ifndef PATH_MANIPULATION_INCLUDE
#define PATH_MANIPULATION_INCLUDE

#include <optional>

#include "environment.h"
#include "path.h"

namespace execHelper::config {
/**
 * Returns a list of the given path and all of its parents in reverse order.
 *
 * \param[in] path  The path to start listing from
 * \returns A list of the given path and its parent paths in reverse order.
 */
[[nodiscard]] auto getAllParentDirectories(Path path) noexcept -> Paths;

/**
 * Returns the path to the home directory if it exists
 *
 * \returns The path to the home directory if it exists
 *          boost::none otherwise
 */
[[nodiscard]] auto getHomeDirectory(const EnvironmentCollection& env) noexcept
    -> std::optional<Path>;
} // namespace execHelper::config

#endif /* PATH_MANIPULATION_INCLUDE */
