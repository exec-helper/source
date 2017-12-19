#ifndef PATH_MANIPULATION_INCLUDE
#define PATH_MANIPULATION_INCLUDE

#include <boost/optional/optional.hpp>

#include "environment.h"
#include "path.h"

namespace execHelper {
namespace config {
/**
 * Returns a list of the given path and all of its parents in reverse order.
 *
 * \param[in] path  The path to start listing from
 * \returns A list of the given path and its parent paths in reverse order.
 */
Paths getAllParentDirectories(Path path) noexcept;

/**
 * Returns the path to the home directory if it exists
 *
 * \returns The path to the home directory if it exists
 *          boost::none otherwise
 */
boost::optional<Path>
getHomeDirectory(const EnvironmentCollection& env) noexcept;
} // namespace config
} // namespace execHelper

#endif /* PATH_MANIPULATION_INCLUDE */
