#include "unittest/catch.h"

#include "config/commandLineOptions.h"

namespace execHelper {
namespace config {
namespace test {
SCENARIO("Test the command line keys", "[config][command-line-options]") {
    REQUIRE(HELP_OPTION_KEY == "help");
    REQUIRE(VERBOSE_KEY == "verbose");
    REQUIRE(DRY_RUN_KEY == "dry-run");
    REQUIRE(JOBS_KEY == "jobs");
    REQUIRE(SETTINGS_FILE_KEY == "settings-file");
    REQUIRE(COMMAND_KEY == "command");
    REQUIRE(LOG_LEVEL_KEY == "debug");
}
} // namespace test
} // namespace config
} // namespace execHelper
