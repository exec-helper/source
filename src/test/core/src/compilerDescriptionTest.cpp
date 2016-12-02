#include <vector>
#include <cassert>
#include <string>

#include <catch.hpp>

#include "core/compilerDescription.h"
#include "compilerStub.h"
#include "modeStub.h"

using std::vector;
using std::string;

using execHelper::core::CompilerDescription;
using execHelper::core::Compiler;
using execHelper::core::Mode;
using execHelper::core::Architecture;

namespace {
    struct CompilerParameters {
        Compiler compiler; 
        Mode mode;
        Architecture architecture;

        CompilerParameters(const Compiler& theCompiler, const Mode& theMode, const Architecture& theArchitecture) :
            compiler(theCompiler),
            mode(theMode),
            architecture(theArchitecture)
        {
            ;
        }
    };
}

namespace execHelper { namespace core { namespace test {
    SCENARIO("Testing the convertTo* functions", "[compiler][CompilerDescription]") {
        GIVEN("Nothin in particular") {
            WHEN("We have a collection of compiler names and the associated compiler collection") {
                vector<string> compilerNames({"gcc", "clang"});
                CompilerDescription::CompilerCollection actualCompilers({Gcc(), Clang()});

                THEN("We should find the corresponding compilers") {
                    CompilerDescription::CompilerCollection foundCompilers = CompilerDescription::convertToCompilerCollection(compilerNames);
                    REQUIRE(foundCompilers == actualCompilers);
                }
            }
            WHEN("We have a wrong compiler name in the collection of compiler names") {
                vector<string> compilerNames({"gcc", "random-compiler", "clang", "other-random-compiler"});
                CompilerDescription::CompilerCollection actualCompilers({Gcc(), Clang()});

                THEN("We should only find the existing corresponding compilers") {
                    CompilerDescription::CompilerCollection foundCompilers = CompilerDescription::convertToCompilerCollection(compilerNames);
                    REQUIRE(foundCompilers == actualCompilers);
                }
            }
            WHEN("We have a collection of mode names and the associated mode collection") {
                vector<string> modeNames({"debug", "release"});
                CompilerDescription::ModeCollection actualModes({Debug(), Release()});

                THEN("We should find the corresponding modes") {
                    CompilerDescription::ModeCollection foundModes = CompilerDescription::convertToModeCollection(modeNames);
                    REQUIRE(foundModes == actualModes);
                }
            }
            WHEN("We have a wrong mode name in the associated mode collection") {
                vector<string> modeNames({"debug", "random-mode", "release", "other-random-mode"});
                CompilerDescription::ModeCollection actualModes({Debug(), Release()});

                THEN("We should only find the existing corresponding modes") {
                    CompilerDescription::ModeCollection foundModes = CompilerDescription::convertToModeCollection(modeNames);
                    REQUIRE(foundModes == actualModes);
                }
            }
        }
    }

    SCENARIO("Creating a compiler description", "[compiler][CompilerDescription]") {
        GIVEN("Certain parameters for the compiler description") {
            CompilerDescription::CompilerCollection actualCompilers = {CompilerStub(), CompilerStub()};
            CompilerDescription::ModeCollection actualModes = {ModeStub(), ModeStub()};
            CompilerDescription::ArchitectureCollection actualArchitectures = {Architecture("i386"), Architecture("armel")};

            WHEN("We create a compiler description with these parameters") {
               CompilerDescription compilerToTest(actualCompilers, actualModes, actualArchitectures); 

                THEN("We should get the same parameters back") {
                    REQUIRE(compilerToTest.getCompilers() == actualCompilers);
                    REQUIRE(compilerToTest.getModes() == actualModes);
                    REQUIRE(compilerToTest.getArchitectures() == actualArchitectures);
                }
            }
        }
    }

    SCENARIO("Creating a compiler description using string collections", "[compiler][CompilerDescription]") {
        GIVEN("Certain parameters for the compiler description") {
            CompilerDescription::CompilerNames compilers({"gcc", "clang"});
            CompilerDescription::ModeNames modes({"debug", "release"});
            CompilerDescription::ArchitectureNames architectures({"i386", "armel"});

            // NOTE: we can only test it this way if the convertTo* functions are tested separately
            CompilerDescription::CompilerCollection actualCompilers = CompilerDescription::convertToCompilerCollection(compilers);
            CompilerDescription::ModeCollection actualModes = CompilerDescription::convertToModeCollection(modes);
            CompilerDescription::ArchitectureCollection actualArchitectures = CompilerDescription::convertToArchitectureCollection(architectures);

            WHEN("We create a compiler description with these parameters") {
               CompilerDescription compilerToTest(compilers, modes, architectures); 

                THEN("We should get the same parameters back") {
                    REQUIRE(compilerToTest.getCompilers() == actualCompilers);
                    REQUIRE(compilerToTest.getModes() == actualModes);
                    REQUIRE(compilerToTest.getArchitectures() == actualArchitectures);
                }
            }
        }
    }


    SCENARIO("Looping over a compiler description", "[compiler][CompilerDescription]") {
        GIVEN("A const compiler collection over which we can loop and all combinations of these") {
            const CompilerDescription compilerDescription({CompilerStub(), CompilerStub()}, {ModeStub(), ModeStub()}, {Architecture("i386"), Architecture("armel")}); 

            vector<CompilerParameters> orderedCombinations;

            // Construct the combination matrix
            for(const auto& compiler : compilerDescription.getCompilers()) {
                for(const auto& mode : compilerDescription.getModes()) {
                    for(const auto& architecture : compilerDescription.getArchitectures()) {
                        CompilerParameters compilerParameters(compiler, mode, architecture);
                        orderedCombinations.push_back(compilerParameters);
                    }
                }
            }

            WHEN("We try too loop over the collection") {
                THEN("We should be able to loop over it using iterators") {
                    size_t orderedCombinationsIndex = 0U;
                    for(CompilerDescription::const_iterator it = compilerDescription.begin(); it != compilerDescription.end(); ++it) {
                        REQUIRE(orderedCombinationsIndex < orderedCombinations.size());

                        const CompilerDescriptionElement iteratedCompilerDescription = *it;
                        REQUIRE(iteratedCompilerDescription.getCompiler() == orderedCombinations[orderedCombinationsIndex].compiler);
                        REQUIRE(iteratedCompilerDescription.getMode() == orderedCombinations[orderedCombinationsIndex].mode);
                        ++orderedCombinationsIndex;
                    } 
                    REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                }
                THEN("We should be able to loop over it using for each loops") {
                    size_t orderedCombinationsIndex = 0U;
                    for(const auto& iteratedCompilerDescription : compilerDescription) {
                        REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                        REQUIRE(iteratedCompilerDescription.getCompiler() == orderedCombinations[orderedCombinationsIndex].compiler);
                        REQUIRE(iteratedCompilerDescription.getMode() == orderedCombinations[orderedCombinationsIndex].mode);
                        ++orderedCombinationsIndex;
                    } 
                    REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                }
            }
        }

        GIVEN("A non-const compiler collection over which we can loop and all combinations of these") {
            CompilerDescription compilerDescription({CompilerStub(), CompilerStub()}, {ModeStub(), ModeStub()}, {Architecture("architectureA"), Architecture("architectureB")}); 

            vector<CompilerParameters> orderedCombinations;

            // Construct the combination matrix
            for(const auto& compiler : compilerDescription.getCompilers()) {
                for(const auto& mode : compilerDescription.getModes()) {
                    for(const auto& architecture : compilerDescription.getArchitectures()) {
                        CompilerParameters compilerParameters(compiler, mode, architecture);
                        orderedCombinations.push_back(compilerParameters);
                    }
                }
            }

            WHEN("We try too loop over the collection") {
                THEN("We should be able to loop over it using iterators") {
                    size_t orderedCombinationsIndex = 0U;
                    for(CompilerDescription::iterator it = compilerDescription.begin(); it != compilerDescription.end(); ++it) {
                        REQUIRE(orderedCombinationsIndex < orderedCombinations.size());

                        const CompilerDescriptionElement iteratedCompilerDescription = *it;
                        REQUIRE(iteratedCompilerDescription.getCompiler() == orderedCombinations[orderedCombinationsIndex].compiler);
                        REQUIRE(iteratedCompilerDescription.getMode() == orderedCombinations[orderedCombinationsIndex].mode);
                        ++orderedCombinationsIndex;
                    } 
                    REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                }
                THEN("We should be able to loop over it using for each loops") {
                    size_t orderedCombinationsIndex = 0U;
                    for(auto iteratedCompilerDescription : compilerDescription) {
                        REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                        REQUIRE(iteratedCompilerDescription.getCompiler() == orderedCombinations[orderedCombinationsIndex].compiler);
                        REQUIRE(iteratedCompilerDescription.getMode() == orderedCombinations[orderedCombinationsIndex].mode);
                        ++orderedCombinationsIndex;
                    } 
                    REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                }
            }
        }
    }

    SCENARIO("Special cases for compiler iteration", "[compiler][CompilerDescription]") {
        GIVEN("Empty collections for the target") {
            const CompilerDescription target((CompilerDescription::CompilerCollection()), CompilerDescription::ModeCollection(), CompilerDescription::ArchitectureCollection()); 

            WHEN("We get the begin iterator") {
                THEN("The begin iterator should be equal to the end iterator") {
                    REQUIRE(target.begin() == target.end());
                }
            }
        }
    }

    SCENARIO("Test the comparisons of the compilers", "[compiler][CompilerDescription]") {
        GIVEN("One test target") {
            CompilerDescription actualCompilerDescription({CompilerStub(), CompilerStub()}, {ModeStub(), ModeStub()}, {Architecture("architectureA"), Architecture("architectureB")});

            WHEN("We compare the target with itself") {
                THEN("We should get it to be equal") {
                    REQUIRE(actualCompilerDescription == actualCompilerDescription);
                    REQUIRE_FALSE(actualCompilerDescription != actualCompilerDescription);
                }
            }
        }
        GIVEN("Two different targets with the same content") {
            CompilerDescription target1({CompilerStub(), CompilerStub()}, {ModeStub(), ModeStub()}, {Architecture("architectureA"), Architecture("architectureB")});
            CompilerDescription target2({CompilerStub(), CompilerStub()}, {ModeStub(), ModeStub()}, {Architecture("architectureA"), Architecture("architectureB")});

            WHEN("We compare the targets") {
                THEN("We should get them to be equal") {
                    REQUIRE(target1 == target2);
                    REQUIRE_FALSE(target1 != target2);
                }
            }
        }
        GIVEN("Two different targets with a different content") {
            CompilerDescription target1({CompilerStub(), CompilerStub()}, {ModeStub(), ModeStub()}, {Architecture("architectureA"), Architecture("architectureB")});
            CompilerDescription target2({CompilerStub()}, {ModeStub(), ModeStub()}, {Architecture("architectureA"), Architecture("architectureB")});
            CompilerDescription target3({CompilerStub(), CompilerStub()}, {ModeStub()}, {});
            CompilerDescription target4((CompilerDescription::CompilerCollection()), {}, {Architecture("architectureA"), Architecture("architectureB")});
            CompilerDescription target5({CompilerStub(), CompilerStub()}, {}, {Architecture("architectureA"), Architecture("architectureB")} );
            CompilerDescription target6({}, {ModeStub(), ModeStub()}, {});

            WHEN("We compare the targets") {
                THEN("We should get them to be not equal") {
                    REQUIRE_FALSE(target1 == target2);
                    REQUIRE_FALSE(target1 == target3);
                    REQUIRE_FALSE(target2 == target3);
                    REQUIRE_FALSE(target1 == target4);
                    REQUIRE_FALSE(target1 == target5);
                    REQUIRE_FALSE(target1 == target6);

                    REQUIRE(target1 != target2);
                    REQUIRE(target1 != target3);
                    REQUIRE(target2 != target3);
                    REQUIRE(target1 != target4);
                    REQUIRE(target1 != target5);
                    REQUIRE(target1 != target6);
                }
            }
        }
    }
} } }
