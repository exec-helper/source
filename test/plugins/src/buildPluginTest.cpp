#include "plugins/buildPlugin.h"

#include <string>

#include "unittest/catch.h"

using std::string;

namespace execHelper {
namespace plugins {
namespace test {
SCENARIO("Test build dir key getter", "[build-plugin]") {
    GIVEN("Nothing in particular") {
        WHEN("We get the build dir key") {
            const string buildDirKey = BuildPlugin::getBuildDirKey();

            THEN("We should find the appropriate one") {
                REQUIRE(buildDirKey == "build-dir");
            }
        }
    }
}
} // namespace test
} // namespace plugins
} // namespace execHelper
