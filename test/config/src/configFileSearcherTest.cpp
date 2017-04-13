#include <experimental/filesystem>
#include <fstream>

#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include <catch.hpp>

#include "config/configFileSearcher.h"

using std::experimental::filesystem::create_directories;
using std::experimental::filesystem::remove_all;
using std::ofstream;
using std::string;

namespace execHelper {
namespace config {
namespace test {
    SCENARIO("Test the config file searcher", "[config][config-file-searcher]") {
        GIVEN("A few search paths and a file to search") {
            const ConfigFileSearcher::SearchPaths searchPaths({"dir1", "dir1/dir2", "dir1/dir2//dir3", "dir1/dir2//dir3//dir4/./"});
            const string settingsFile("test-settings-file");

            ConfigFileSearcher configFileSearcher(searchPaths);

            WHEN("We write the settings file") {
                // Clear in case certain of these files are still lurking around
                for(auto it = searchPaths.rbegin(); it != searchPaths.rend(); ++it) {
                    const string fileToDelete = *it + "/" + settingsFile;
                    remove(fileToDelete.c_str());
                }

                for(auto it = searchPaths.rbegin(); it != searchPaths.rend(); ++it) {
                    create_directories(*it);
                }

                for(auto it = searchPaths.rbegin(); it != searchPaths.rend(); ++it) {
                    const string correctFilePath = *it;
                    ofstream file;
                    string filename(correctFilePath);
                    filename += "/";
                    filename += settingsFile;

                    file.open(filename, std::ios::out | std::ios::trunc);
                    file << "test.exec-helper" << std::endl;
                    file.close();

                    THEN("It should find it") {
                        boost::optional<std::string> result = configFileSearcher.find(settingsFile);
                        REQUIRE(result != boost::none);
                        REQUIRE(result.get() == filename);
                    }
                }

                // Clean up after ourselves
                for(auto it = searchPaths.rbegin(); it != searchPaths.rend(); ++it) {
                    const string fileToDelete = *it + "/" + settingsFile;
                    REQUIRE(remove_all(fileToDelete));
                }
            }

            WHEN("We do not write the settings file") {
                THEN("It should not find it") {
                    boost::optional<std::string> result = configFileSearcher.find(settingsFile);
                    REQUIRE(result == boost::none);
                }
            }
        }
    }
} // test 
} // config
} // execHelper
