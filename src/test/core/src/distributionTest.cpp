#include <catch.hpp>
#include <string>

#include "core/distribution.h"

using std::string;

namespace execHelper { namespace core { namespace test {
    SCENARIO("Basic distribution test", "[distribution]") {
        GIVEN("A distribution object") {
            string distributionName("distribution1");
            Distribution distribution(distributionName);

            WHEN("We check its member functions") {
                THEN("We should get the specifics of the Debug distribution object") {
                    REQUIRE(distribution.getDistribution() == distributionName);
                }
            }
        }
        GIVEN("Two distribution objects of the same type") {
            string distributionName("distribution1");
            Distribution distribution1(distributionName);
            Distribution distribution2(distributionName);

            WHEN("We compare the two objects") {
                THEN("They should be equal") {
                    REQUIRE(distribution1 == distribution2);
                }
            }
        }
        GIVEN("Two different distribution objects") {
            Distribution distribution1("distribution1");
            Distribution distribution2("distribution2");

            WHEN("We compare the two objects") {
                THEN("They should not be equal") {
                    REQUIRE(distribution1 != distribution2);
                }
            }
        }
    }
} } }
