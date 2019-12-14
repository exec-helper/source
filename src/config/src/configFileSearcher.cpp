#include "configFileSearcher.h"

#include <filesystem>

using std::optional;
using std::string;

namespace filesystem = std::filesystem;

namespace {
/**
 * Checks whether the given path exists
 *
 * \param[in] pathToCheck The path to check
 * \returns True    If the path exists on the current system
 *          False   Otherwise
 */
auto fileExist(const execHelper::config::Path& pathToCheck) noexcept -> bool {
    return filesystem::exists(pathToCheck);
}
} // namespace

namespace execHelper::config {
ConfigFileSearcher::ConfigFileSearcher(Paths searchPaths) noexcept
    : m_searchPaths(std::move(searchPaths)) {
    ;
}

auto ConfigFileSearcher::find(const Path& filename) noexcept -> optional<Path> {
    for(const auto& searchPath : m_searchPaths) {
        Path pathToCheck = searchPath;
        pathToCheck /= filename;
        if(fileExist(pathToCheck)) {
            return pathToCheck;
        }
    }
    return std::nullopt;
}
} // namespace execHelper::config
