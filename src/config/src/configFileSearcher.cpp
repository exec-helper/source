#include "configFileSearcher.h"

#include <fstream>

using std::ifstream;
using std::string;

namespace execHelper {
    namespace config {
        ConfigFileSearcher::ConfigFileSearcher(SearchPaths searchPaths) noexcept :
            m_searchPaths(std::move(searchPaths))
        {
            ;
        }

        boost::optional<std::string> ConfigFileSearcher::find(const std::string& filename) noexcept {
            for(const auto& searchPath : m_searchPaths) {
                const string path = searchPath + "/" + filename;
                if(fileExist(searchPath + "/" + filename)) {
                    return path;
                }
            }
            return boost::none;
        }

        bool ConfigFileSearcher::fileExist(const string& path) noexcept {
            ifstream infile(path);
            return infile.good();
        }
    } // config
} // execHelper
