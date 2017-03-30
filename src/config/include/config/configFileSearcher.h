#ifndef CONFIG_FILE_SEARCHER_INCLUDE
#define CONFIG_FILE_SEARCHER_INCLUDE

#include <vector>

#include <boost/optional/optional.hpp>

namespace execHelper {
    namespace config {
        class ConfigFileSearcher {
            public:
                typedef std::string SearchPath;
                typedef std::vector<SearchPath> SearchPaths;

                ConfigFileSearcher(SearchPaths searchPaths) noexcept;
                boost::optional<std::string> find(const std::string& filename) noexcept;

            private:
                bool fileExist(const std::string& path) noexcept;

                const SearchPaths m_searchPaths;
        };
    } // config
} // execHelper


#endif  /* CONFIG_FILE_SEARCHER_INCLUDE */
