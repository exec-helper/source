#include "pathManipulation.h"

#include <string>

#include <boost/filesystem.hpp>

using std::string;

using boost::filesystem::absolute;
using boost::filesystem::is_regular_file;

namespace execHelper {
namespace config {
Paths getAllParentDirectories(Path path) noexcept {
    if(path.is_relative()) {
        path = absolute(path);
    }

    if(is_regular_file(path)) {
        path = path.parent_path();
    }

    Paths parentPaths;
    while(path != path.parent_path()) {
        parentPaths.push_back(path);
        path = path.parent_path();
    }
    parentPaths.push_back(path.parent_path());
    return parentPaths;
}

std::optional<Path>
getHomeDirectory(const EnvironmentCollection& env) noexcept {
    const string HOME_DIR_KEY("HOME");
    if(env.count(HOME_DIR_KEY) == 0) {
        return std::nullopt;
    }
    return Path(env.at(HOME_DIR_KEY));
}
} // namespace config
} // namespace execHelper
