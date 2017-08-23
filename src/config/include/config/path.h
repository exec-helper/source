#ifndef PATH_INCLUDE
#define PATH_INCLUDE

#include <boost/filesystem/path.hpp>

namespace execHelper {
    namespace config {
        using Path = boost::filesystem::path;
        using Paths = std::vector<Path>;
    } // namespace config
} // namespace execHelper

#endif  /* PATH_INCLUDE */
