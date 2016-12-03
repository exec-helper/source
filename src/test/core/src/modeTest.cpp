#include <catch.hpp>
#include <string>

#include "core/mode.h"

using std::string;

namespace execHelper { namespace core { namespace test {
    SCENARIO("Basic mode test", "[mode]") {
        GIVEN("A mode object") {
            string modeName("mode1");
            Mode mode(modeName);

            WHEN("We check its member functions") {
                THEN("We should get the specifics of the Debug mode object") {
                    REQUIRE(mode.getMode() == modeName);
                }
            }
        }
        GIVEN("Two mode objects of the same type") {
            string modeName("mode1");
            Mode mode1(modeName);
            Mode mode2(modeName);

            WHEN("We compare the two objects") {
                THEN("They should be equal") {
                    REQUIRE(mode1 == mode2);
                }
            }
        }
        GIVEN("Two different mode objects") {
            Mode mode1("mode1");
            Mode mode2("mode2");

            WHEN("We compare the two objects") {
                THEN("They should not be equal") {
                    REQUIRE(mode1 != mode2);
                }
            }
        }
    }
} } }
