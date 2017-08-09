#include "configFileSearcher.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

using std::string;

using boost::filesystem::exists;
using boost::filesystem::ifstream;
using boost::filesystem::path;

namespace execHelper {
    namespace config {
        ConfigFileSearcher::ConfigFileSearcher(Paths searchPaths) noexcept :
            m_searchPaths(std::move(searchPaths))
        {
            ;
        }

        boost::optional<Path> ConfigFileSearcher::find(const Path& filename) noexcept {
            for(const auto& searchPath : m_searchPaths) {
                Path pathToCheck = searchPath;
                pathToCheck /= filename;
                if(fileExist(pathToCheck)) {
                    return pathToCheck;
                }
            }
            return boost::none;
        }

        bool ConfigFileSearcher::fileExist(const Path& pathToCheck) noexcept {
            return exists(pathToCheck);
        }
    } // config
} // execHelper
