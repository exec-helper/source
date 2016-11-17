#include <catch.hpp>

#include "core/mode.h"

namespace execHelper { namespace core { namespace test {
    SCENARIO("Basic debug mode test", "[mode]") {
        GIVEN("A debug mode object") {
            Debug debug;

            WHEN("We check its member functions") {
                THEN("We should get the specifics of the Debug mode object") {
                    REQUIRE(debug.getMode() == "debug");
                }
            }
        }
        GIVEN("Two debug mode objects") {
            Debug debug1;
            Debug debug2;

            WHEN("We compare the two objects") {
                THEN("They should be equal") {
                    REQUIRE(debug1 == debug2);
                }
            }
        }
        GIVEN("A debug mode and an other object") {
            Debug debug;
            Release release;

            WHEN("We compare the two objects") {
                THEN("They should not be equal") {
                    REQUIRE(debug != release);
                }
            }
        }
    }
    SCENARIO("Basic release mode test", "[mode]") {
        GIVEN("A release mode object") {
            Release release;

            WHEN("We check its member functions") {
                THEN("We should get the specifics of the Debug mode object") {
                    REQUIRE(release.getMode() == "release");
                }
            }
        }
        GIVEN("Two release mode objects") {
            Release release1;
            Release release2;

            WHEN("We compare the two objects") {
                THEN("They should be equal") {
                    REQUIRE(release1 == release2);
                }
            }
        }
        GIVEN("A release mode and an other object") {
            Release release;
            Debug debug;

            WHEN("We compare the two objects") {
                THEN("They should not be equal") {
                    REQUIRE(release != debug);
                }
            }
        }
    }

} } }
