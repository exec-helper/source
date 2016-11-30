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
            const Gcc compiler;
            const CompilerDescription::CompilerCollection compilers({compiler});
            const Release mode;
            const CompilerDescription::ModeCollection modes({mode});
            const CompilerDescription compilerDescription(compilers, modes);
            const Patterns patterns({compilerPattern, modePattern});

            const string beginString = string("test/{") + compilerPattern + "}/blaat/{" + modePattern + "}/{HELLO}";
            const string actualString = string("test/") + compiler.getName() + "/blaat/" + mode.getMode() + "/{HELLO}";

            WHEN("We replace the patterns in the begin string") {
                string resultingString = replacePatterns(beginString, patterns, compilerDescription); 

                THEN("We should get the replaced result") {
                    REQUIRE(resultingString == actualString);
                }
            }
        }

    }
} } }
