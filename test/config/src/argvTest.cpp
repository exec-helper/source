#include <array>
#include <sstream>
#include <vector>

#include <gsl/string_span>

#include "config/argv.h"
#include "unittest/catch.h"

using std::array;
using std::move;
using std::string;
using std::stringstream;
using std::vector;

using gsl::czstring;

namespace execHelper {
    namespace config {
        namespace test {
            SCENARIO("Test argv construction", "[config][argv]") {
                GIVEN("An array of string literals") {
                    const int argc = 3U;
                    array<czstring<>, argc + 1U> argv = {{"Hello", "world", "!!!", ""}};

                    WHEN("We create the argv") {
                        Argv args(argc, &(argv[0]));

                        THEN("The number of arguments should match") {
                            REQUIRE(argc == args.getArgc());
                        }

                        THEN("The associated argv char array should be returned") {
                            char** returnedArgv = args.getArgv(); 
                            for(int i = 0U; i < argc; ++i) {
                                REQUIRE(returnedArgv[i] == args[i]);    // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            }
                        }

                        THEN("argv[argc] should be a nullptr") {
                            char** returnedArgv = args.getArgv(); 
                            REQUIRE(returnedArgv[argc] == nullptr);     // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        }

                        THEN("We should find the right elements using the access operator") {
                            for(size_t i = 0U; i < argc; ++i) {
                                REQUIRE(string(args[i]) == string(argv[i]));    // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
                            }
                        }
                    }

                    WHEN("We create the const argv") {
                        const Argv args(argc, &argv[0]);

                        THEN("The number of arguments should match") {
                            REQUIRE(argc == args.getArgc());
                        }

                        THEN("The associated argv char array should be returned") {
                            const char* const * returnedArgv = args.getArgv(); 
                            for(int i = 0U; i < argc; ++i) {
                                REQUIRE(returnedArgv[i] == args[i]);    // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            }
                        }

                        THEN("argv[argc] should be a nullptr") {
                            const char* const * returnedArgv = args.getArgv(); 
                            REQUIRE(returnedArgv[argc] == nullptr);     // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        }

                        THEN("We should find the right elements using the access operator") {
                            for(size_t i = 0U; i < argc; ++i) {
                                REQUIRE(string(args[i]) == string(argv[i]));    // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
                            }
                        }
                    }
                }
                GIVEN("A taskcollection to take the arguments from") {
                    const vector<string> args({"arg1", "arg2", "arg3"});

                    WHEN("We create the argv") {
                        Argv argv(args);

                        THEN("The number of arguments should match") {
                            REQUIRE(args.size() == argv.getArgc());
                        }

                        THEN("The associated argv char array should be returned") {
                            char** returnedArgv = argv.getArgv(); 
                            for(size_t i = 0U; i < args.size(); ++i) {
                                REQUIRE(returnedArgv[i] == args[i]);    // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            }
                        }

                        THEN("argv[argc] should be a nullptr") {
                            char** returnedArgv = argv.getArgv(); 
                            REQUIRE(returnedArgv[args.size()] == nullptr);  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        }

                        THEN("We should find the right elements using the access operator") {
                            for(size_t i = 0U; i < args.size(); ++i) {
                                REQUIRE(args[i] == argv[i]);
                            }
                        }
                    }
                    WHEN("We create the const argv") {
                        const Argv argv(args);

                        THEN("The number of arguments should match") {
                            REQUIRE(args.size() == argv.getArgc());
                        }

                        THEN("The associated argv char array should be returned") {
                            const char* const * returnedArgv = argv.getArgv(); 
                            for(size_t i = 0U; i < args.size(); ++i) {
                                REQUIRE(returnedArgv[i] == args[i]);    // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            }
                        }

                        THEN("argv[argc] should be a nullptr") {
                            const char* const * returnedArgv = argv.getArgv(); 
                            REQUIRE(returnedArgv[args.size()] == nullptr);  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        }

                        THEN("We should find the right elements using the access operator") {
                            for(size_t i = 0U; i < args.size(); ++i) {
                                REQUIRE(args[i] == argv[i]);
                            }
                        }
                    }
                }      
            }

            SCENARIO("Test the argv copy and move constructor, assignment operators and the swap operator", "[config][argv]") {
                GIVEN("An argv object to copy") {
                    const vector<string> args({"arg1", "arg2", "arg3", "arg4"});
                    Argv argv(args);

                    WHEN("We copy the given object") {
                        Argv copy(argv);    // NOLINT(performance-unnecessary-copy-initialization)

                        // Save the pointers to the arguments
                        vector<char*> argvAddresses;
                        for(size_t i = 0U; i < argv.getArgc(); ++i) {
                            argvAddresses.push_back(argv[i]);
                        }

                        THEN("The values of the pointers must be equal") {
                            REQUIRE(args.size() == copy.getArgc());
                            for(size_t i = 0U; i < args.size(); ++i) {
                                REQUIRE(args[i] == copy[i]);
                            }
                        }

                        THEN("It must have been a deep copy/the actual pointer must be different") {
                            REQUIRE(argvAddresses.size() == copy.getArgc());
                            for(size_t i = 0U; i < argvAddresses.size(); ++i) {
                                REQUIRE(argvAddresses[i] != copy[i]);
                            }
                        }
                    }
                    WHEN("We copy assign the given object") {
                        Argv assign(vector<string>({"copy-assign1", "copy-assign2"}));
                        assign = argv;

                        // Save the pointers to the arguments
                        vector<char*> argvAddresses;
                        for(size_t i = 0U; i < argv.getArgc(); ++i) {
                            argvAddresses.push_back(argv[i]);
                        }

                        THEN("They must be equal") {
                            REQUIRE(args.size() == assign.getArgc());
                            for(size_t i = 0U; i < args.size(); ++i) {
                                REQUIRE(args[i] == assign[i]);
                            }
                        }

                        THEN("It must have been a deep copy") {
                            REQUIRE(argvAddresses.size() == assign.getArgc());
                            for(size_t i = 0U; i < argvAddresses.size(); ++i) {
                                REQUIRE(argvAddresses[i] != assign[i]);
                            }
                        }
                    }
                    WHEN("We move the given object") {
                        Argv moved(move(argv));
                        THEN("We must find the expected content") {
                            REQUIRE(args.size() == moved.getArgc());
                            for(size_t i = 0U; i < args.size(); ++i) {
                                REQUIRE(args[i] == moved[i]);
                            }
                        }
                    }
                    WHEN("We move assign the given object") {
                        Argv assign(vector<string>({"move-assign1", "move-assign2"}));
                        assign = move(argv);    // NOLINT(hicpp-invalid-access-moved)
                        THEN("We must find the expected content") {
                            REQUIRE(args.size() == assign.getArgc());
                            for(size_t i = 0U; i < args.size(); ++i) {
                                REQUIRE(args[i] == assign[i]);
                            }
                        }
                    }
                    WHEN("We swap the given object") {
                        const vector<string> swappedContent({"swap1", "swap2", "swap3"});
                        Argv swapped(swappedContent);
                        argv.swap(swapped);     // NOLINT(hicpp-invalid-access-moved)
                        THEN("We must find the expected content for the first object") {
                            REQUIRE(argv.getArgc() == swappedContent.size());
                            for(size_t i = 0U; i < swappedContent.size(); ++i) {
                                REQUIRE(argv[i] == swappedContent[i]);
                            }
                        }
                        THEN("We must find the swapped content for the second object") {
                            REQUIRE(args.size() == swapped.getArgc());
                            for(size_t i = 0U; i < args.size(); ++i) {
                                REQUIRE(args[i] == swapped[i]);
                            }
                        }
                    }
                }
            }

            SCENARIO("Test the argv equality operators", "[config][argv]") {
                GIVEN("An argv object to compare with") {
                    const vector<string> args({"eq1", "eq2"});
                    const Argv argv(args);

                    WHEN("We compare it with itself") {
                        THEN("It should compare equal") {
                            REQUIRE(argv == argv);
                            REQUIRE_FALSE(argv != argv);
                        }
                    }

                    WHEN("We copy the object and compare") {
                        const Argv copy(argv);  // NOLINT(performance-unnecessary-copy-initialization)

                        THEN("They should not be equal") {
                            // Note: this is due to the fact that we expect the copy constructor
                            // to make a deep copy
                            REQUIRE(argv != copy);
                            REQUIRE_FALSE(argv == copy);
                        }
                    }

                    WHEN("We create an object with the same parameters") {
                        const Argv same(args);

                        THEN("They should not compare equal") {
                            REQUIRE(argv != same);
                            REQUIRE_FALSE(argv == same);
                        }
                    }
                }
            }

            SCENARIO("Test the argv streaming operator", "[config][argv]") {
                GIVEN("An argv object to stream") {
                    const vector<string> args({"stream1", "stream2", "stream3"});
                    const Argv argv(args);

                    WHEN("We stream the argv object") {
                        stringstream stream;
                        stream << argv;

                        THEN("We must find the expected one") {
                            stringstream correctStream;
                            for(const auto& arg : args) {
                                if(arg != args.front()) {
                                    correctStream << ", ";
                                }
                                correctStream << arg;
                            }
                            REQUIRE(stream.str() == correctStream.str());
                        }
                    }
                }
            }

            SCENARIO("Error conditions for the access operator", "[config][argv]") {
                GIVEN("An empty object") {
                    Argv argv(vector<string>({"arg1", "arg2", "arg3"}));

                    WHEN("We access an out of bounds index") {
                        auto receivedArg = argv[argv.getArgc()];

                        THEN("We should receive a nullptr") {
                            REQUIRE(receivedArg == nullptr);
                        }
                    }
                }
            }
        } // namespace test
    } // namespace config
} // namespace execHelper
