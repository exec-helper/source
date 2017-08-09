#ifndef CONFIG_FILE_SEARCHER_INCLUDE
#define CONFIG_FILE_SEARCHER_INCLUDE

#include <vector>

#include <boost/optional/optional.hpp>

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
                 * \param searchPaths  The paths to search for ordered by which paths to look in first.
                 */
                ConfigFileSearcher(Paths searchPaths) noexcept;

                /**
                 * Find the given filename
                 *
                 * \param filename The filename to look for in the given search paths
                 * \returns The path in search path with the highest importance under which the filename was found.
                 *          boost::none Otherwise
                 */
                boost::optional<Path> find(const Path& filename) noexcept;

            private:

                /**
                 * Checks whether the given path exists
                 *
                 * \param pathToCheck The path to check
                 * \returns True    If the path exists on the current system
                 *          False   Otherwise
                 */
                bool fileExist(const Path& pathToCheck) noexcept;

                const Paths m_searchPaths;
        };
    } // config
} // execHelper


#endif  /* CONFIG_FILE_SEARCHER_INCLUDE */
