#ifndef PATH_INCLUDE
#define PATH_INCLUDE

#include <filesystem>
#include <vector>

namespace execHelper {
namespace config {
using Path = std::filesystem::path;
using Paths = std::vector<Path>;
} // namespace config
} // namespace execHelper

#endif /* PATH_INCLUDE */
