#include "pathManipulation.h"

#include <string>

using std::string;

namespace execHelper::config {
auto getAllParentDirectories(Path path) noexcept -> Paths {
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

auto getHomeDirectory(const EnvironmentCollection& env) noexcept
    -> std::optional<Path> {
    const string HOME_DIR_KEY("HOME");
    if(env.count(HOME_DIR_KEY) == 0) {
        return std::nullopt;
    }
    return Path(env.at(HOME_DIR_KEY));
}
} // namespace execHelper::config
