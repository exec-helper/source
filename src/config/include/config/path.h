#ifndef PATH_INCLUDE
#define PATH_INCLUDE

#include <boost/filesystem/path.hpp>

namespace execHelper {
    namespace config {
        typedef boost::filesystem::path Path;
        typedef std::vector<Path> Paths;
    } // config
} // execHelper


#endif  /* PATH_INCLUDE */
