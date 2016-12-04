#include <catch.hpp>
#include <string>

#include "core/architecture.h"

using std::string;

namespace execHelper { namespace core { namespace test {
    SCENARIO("Basic architecture test", "[architecture]") {
        GIVEN("A architecture object") {
            string architectureName("architecture1");
            Architecture architecture(architectureName);

            WHEN("We check its member functions") {
                THEN("We should get the specifics of the Debug architecture object") {
                    REQUIRE(architecture.getArchitecture() == architectureName);
                }
            }
        }
        GIVEN("Two architecture objects of the same type") {
            string architectureName("architecture1");
            Architecture architecture1(architectureName);
            Architecture architecture2(architectureName);

            WHEN("We compare the two objects") {
                THEN("They should be equal") {
                    REQUIRE(architecture1 == architecture2);
                }
            }
        }
        GIVEN("Two different architecture objects") {
            Architecture architecture1("architecture1");
            Architecture architecture2("architecture2");

            WHEN("We compare the two objects") {
                THEN("They should not be equal") {
                    REQUIRE(architecture1 != architecture2);
                }
            }
        }
    }
} } }
