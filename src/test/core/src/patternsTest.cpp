#include <catch.hpp>
#include <string>

#include "core/patterns.h"
#include "core/compiler.h"
#include "core/compilerDescription.h"

using std::string;
using execHelper::core::CompilerDescription;

namespace execHelper { namespace core { namespace test {
    SCENARIO("Test pattern replacement", "[patterns]") {
        GIVEN("A string to replace patterns in") {
            const string compilerPattern("COMPILER");
            const string modePattern("MODE");
            const string realCompiler("compiler1");
            const string realMode("mode1");

            const string beginString = string("test/{") + compilerPattern + "}/blaat/{" + modePattern + "}/{HELLO}";
            const string actualString = string("test/") + realCompiler + "/blaat/" + realMode + "/{HELLO}";

            WHEN("We replace the patterns in the begin string") {
                string resultingString = replacePatterns(beginString, compilerPattern, realCompiler); 
                resultingString = replacePatterns(resultingString, modePattern, realMode); 

                THEN("We should get the replaced result") {
                    REQUIRE(resultingString == actualString);
                }
            }
        }
        GIVEN("A string to replace patterns in") {
            const string compilerPattern("COMPILER");
            const string modePattern("MODE");
            const string architecturePattern("ARCHITECTURE");

            const string compilerName("compiler1");
            const string architectureName("architectureA");

            const Compiler compiler(compilerName);
            const Release mode;
            const Architecture architecture(architectureName);

            const CompilerDescriptionElement compilerDescriptionElement(compiler, mode, architecture);
            const Patterns patterns({compilerPattern, modePattern, architecturePattern});

            const string beginString = string("test/{") + compilerPattern + "}/blaat/{" + modePattern + "}/{HELLO}/{" + architecturePattern + "}";
            const string actualString = string("test/") + compilerName + "/blaat/" + mode.getMode() + "/{HELLO}/" + architectureName;

            WHEN("We replace the patterns in the begin string") {
                string resultingString = replacePatterns(beginString, patterns, compilerDescriptionElement); 

                THEN("We should get the replaced result") {
                    REQUIRE(resultingString == actualString);
                }
            }
        }

    }
} } }
