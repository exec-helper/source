#include "config/configFileSearcher.h"
#include "unittest/catch.h"
#include <filesystem>
#include <fstream>

using std::ofstream;
using std::string;

namespace filesystem = std::filesystem;

namespace execHelper::config::test {
SCENARIO("Test the config file searcher", "[config][config-file-searcher]") {
    GIVEN("A few search paths and a file to search") {
        const Paths searchPaths({"dir1", "dir1/dir2", "dir1/dir2//dir3",
                                 "dir1/dir2//dir3//dir4/./"});
        const Path settingsFile("test-settings-file");

        ConfigFileSearcher configFileSearcher(searchPaths);

        WHEN("We write the settings file") {
            // Clear in case certain of these files are still lurking around
            for(const auto& searchPath : searchPaths) {
                Path fileToDelete(searchPath);
                fileToDelete /= settingsFile;
                remove(fileToDelete.c_str());
            }

            for(const auto& searchPath : searchPaths) {
                create_directories(searchPath);
            }

            for(const auto& searchPath : searchPaths) {
                ofstream file;
                filesystem::path filename(searchPath);
                filename /= settingsFile;

                file.open(filename, std::ios::out | std::ios::trunc);
                file << "test.exec-helper" << std::endl;
                file.close();

                THEN("It should find it") {
                    auto result = configFileSearcher.find(settingsFile);
                    REQUIRE(result != std::nullopt);
                    REQUIRE(result.value() == filename);
                }
            }

            // Clean up after ourselves
            for(const auto& searchPath : searchPaths) {
                remove_all(searchPath);
            }
        }

        WHEN("We do not write the settings file") {
            THEN("It should not find it") {
                auto result = configFileSearcher.find(settingsFile);
                REQUIRE(result == std::nullopt);
            }
        }
    }
}
} // namespace execHelper::config::test
