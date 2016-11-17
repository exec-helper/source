#include <catch.hpp>

#include "core/compiler.h"

namespace execHelper { namespace core { namespace test {
    SCENARIO("Basic gcc compiler test", "[compiler]") {
        GIVEN("A gcc compiler object") {
            Gcc gcc;

            WHEN("We check its member functions") {
                THEN("We should get the specifics of the Gcc compiler object") {
                    REQUIRE(gcc.getName() == "gcc");
                }
            }
        }
        GIVEN("Two gcc compiler objects") {
            Gcc gcc1;
            Gcc gcc2;

            WHEN("We compare the two objects") {
                THEN("They should be equal") {
                    REQUIRE(gcc1 == gcc2);
                }
            }
        }
        GIVEN("A gcc compiler and an other object") {
            Gcc gcc;
            Clang clang;

            WHEN("We compare the two objects") {
                THEN("They should not be equal") {
                    REQUIRE(gcc != clang);
                }
            }
        }
    }
    SCENARIO("Basic clang compiler test", "[compiler]") {
        GIVEN("A clang compiler object") {
            Clang clang;

            WHEN("We check its member functions") {
                THEN("We should get the specifics of the Gcc compiler object") {
                    REQUIRE(clang.getName() == "clang");
                }
            }
        }
        GIVEN("Two clang compiler objects") {
            Clang clang1;
            Clang clang2;

            WHEN("We compare the two objects") {
                THEN("They should be equal") {
                    REQUIRE(clang1 == clang2);
                }
            }
        }
        GIVEN("A clang compiler and an other object") {
            Clang clang;
            Gcc gcc;

            WHEN("We compare the two objects") {
                THEN("They should not be equal") {
                    REQUIRE(clang != gcc);
                }
            }
        }
    }

} } }
