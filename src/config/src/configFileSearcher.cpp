#include "configFileSearcher.h"

#include <filesystem>

using std::optional;
using std::string;

namespace filesystem = std::filesystem;

namespace execHelper::config {
// cppcheck-suppress passedByValue symbolName=searchPaths
ConfigFileSearcher::ConfigFileSearcher(Paths searchPaths) noexcept
    : m_searchPaths(std::move(searchPaths)) {
    ;
}

optional<Path> ConfigFileSearcher::find(const Path& filename) noexcept {
    for(const auto& searchPath : m_searchPaths) {
        Path pathToCheck = searchPath;
        pathToCheck /= filename;
        if(fileExist(pathToCheck)) {
            return pathToCheck;
        }
    }
    return std::nullopt;
}

bool ConfigFileSearcher::fileExist(const Path& pathToCheck) noexcept {
    return filesystem::exists(pathToCheck);
}
} // namespace execHelper::config
