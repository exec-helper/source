#include <catch.hpp>

#include <string>

#include "core/compiler.h"

using std::string;

namespace execHelper { namespace core { namespace test {
    SCENARIO("Basic gcc compiler test", "[compiler]") {
        GIVEN("A compiler object") {
            string compilerName("gcc");
            Compiler compiler(compilerName);

            WHEN("We check its member functions") {
                THEN("We should get the specifics of the Gcc compiler object") {
                    REQUIRE(compiler.getName() == compilerName);
                }
            }
        }
        GIVEN("Two compiler objects with the same name") {
            string compilerName("gcc");
            Compiler compiler1(compilerName);
            Compiler compiler2(compilerName);

            WHEN("We compare the two objects") {
                THEN("They should be equal") {
                    REQUIRE(compiler1 == compiler2);
                }
            }
        }
        GIVEN("Two different compilers") {
            string compiler1Name("gcc");
            string compiler2Name("clang");
            Compiler compiler1(compiler1Name);
            Compiler compiler2(compiler2Name);

            WHEN("We compare the two objects") {
                THEN("They should not be equal") {
                    REQUIRE(compiler1 != compiler2);
                }
            }
        }
    }
} } }
