#ifndef CONFIG_FILE_SEARCHER_INCLUDE
#define CONFIG_FILE_SEARCHER_INCLUDE

#include <optional>
#include <vector>

#include "path.h"

namespace execHelper {
namespace config {
/**
 * \brief Searches the config in the order of the given search paths
 */
class ConfigFileSearcher {
  public:
    /**
     * Constructs a config file searcher
     *
     * \param[in] searchPaths  The paths to search for ordered by which paths to
     * look in first.
     */
    explicit ConfigFileSearcher(Paths searchPaths) noexcept;

    /**
     * Find the given filename
     *
     * \param[in] filename The filename to look for in the given search paths
     * \returns The path in search path with the highest importance under which
     * the filename was found. std::nullopt Otherwise
     */
    std::optional<Path> find(const Path& filename) noexcept;

  private:
    /**
     * Checks whether the given path exists
     *
     * \param[in] pathToCheck The path to check
     * \returns True    If the path exists on the current system
     *          False   Otherwise
     */
    bool fileExist(const Path& pathToCheck) noexcept;

    const Paths m_searchPaths;
};
} // namespace config
} // namespace execHelper

#endif /* CONFIG_FILE_SEARCHER_INCLUDE */
