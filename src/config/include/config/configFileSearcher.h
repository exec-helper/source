#ifndef CONFIG_FILE_SEARCHER_INCLUDE
#define CONFIG_FILE_SEARCHER_INCLUDE

#include <vector>

#include <boost/optional/optional.hpp>

namespace execHelper {
    namespace config {
        /**
         * \brief Searches the config in the order of the given search paths
         */
        class ConfigFileSearcher {
            public:
                typedef std::string SearchPath;
                typedef std::vector<SearchPath> SearchPaths;

                /**
                 * Constructs a config file searcher
                 *
                 * \param searchPaths  The paths to search for ordered by which paths to look in first.
                 */
                ConfigFileSearcher(SearchPaths searchPaths) noexcept;

                /**
                 * Find the given filename
                 *
                 * \param filename The filename to look for in the given search paths
                 * \returns The path in search path with the highest importance under which the filename was found.
                 *          boost::none Otherwise
                 */
                boost::optional<std::string> find(const std::string& filename) noexcept;

            private:

                /**
                 * Checks whether the given path exists
                 *
                 * \param path The path to check
                 * \returns True    If the path exists on the current system
                 *          False   Otherwise
                 */
                bool fileExist(const std::string& path) noexcept;

                const SearchPaths m_searchPaths;
        };
    } // config
} // execHelper


#endif  /* CONFIG_FILE_SEARCHER_INCLUDE */
