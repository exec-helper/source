#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/envp.h"
#include "unittest/catch.h"

using gsl::czstring;

using std::move;
using std::string;
using std::stringstream;
using std::vector;

namespace execHelper::config::test {
SCENARIO("Test envp construction", "[config][envp]") {
    GIVEN("A taskcollection to take the arguments from") {
        const EnvironmentCollection env(
            {{"key1", "value1"}, {"key2", "value2"}, {"key3", "value3"}});
        vector<string> expectedEnv;
        expectedEnv.reserve(env.size());
        for(const auto& element : env) {
            expectedEnv.emplace_back(
                string(element.first).append("=").append(element.second));
        }

        WHEN("We create the envp") {
            Envp envp(env);

            THEN("The number of arguments should match") {
                REQUIRE(env.size() == envp.size());
            }

            THEN("The associated envp char array should be returned") {
                char** returnedEnvp = envp.getEnvp();
                size_t index = 0U;
                while(
                    returnedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index] != nullptr) {
                    REQUIRE(index < env.size());
                    REQUIRE(
                        returnedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            [index] == expectedEnv[index]);
                    ++index;
                }
            }
        }
        WHEN("We create the const envp") {
            const Envp envp(env);

            THEN("The number of arguments should match") {
                REQUIRE(expectedEnv.size() == envp.size());
            }

            THEN("The associated envp char array should be returned") {
                const char* const* returnedEnvp = envp.getEnvp();
                size_t index = 0U;
                while(
                    returnedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index] != nullptr) {
                    REQUIRE(index < expectedEnv.size());
                    REQUIRE(
                        returnedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            [index] == expectedEnv[index]);
                    ++index;
                }
            }
        }
    }
}

SCENARIO("Test the envp copy and move constructor, assignment operators and "
         "the swap operator",
         "[config][envp][check]") {
    GIVEN("An envp object to copy") {
        const EnvironmentCollection env(
            {{"key1", "value1"}, {"key2", "value2"}, {"key3", "value3"}});
        vector<string> expectedEnv;
        expectedEnv.reserve(env.size());
        for(const auto& element : env) {
            expectedEnv.emplace_back(
                string(element.first).append("=").append(element.second));
        }

        Envp envp(env);

        WHEN("We copy the given object") {
            Envp copy(envp);

            // Save the pointers to the arguments
            vector<const char*> envpAddresses;
            const char* const* returnedEnvp = envp.getEnvp();
            size_t index = 0U;
            while(
                returnedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    [index] != nullptr) {
                envpAddresses.push_back(
                    returnedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index]);
                ++index;
            }

            THEN("The values of the pointers must be equal") {
                const char* const* copiedEnvp = copy.getEnvp();
                size_t index = 0U;
                while(
                    copiedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index] != nullptr) {
                    REQUIRE(
                        expectedEnv[index] ==
                        copiedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            [index]);
                    ++index;
                }
                REQUIRE(index == expectedEnv.size());
            }

            THEN("It must have been a deep copy/the actual pointer must be "
                 "different") {
                const char* const* copiedEnvp = copy.getEnvp();
                size_t index = 0U;
                while(
                    copiedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index] != nullptr) {
                    REQUIRE(
                        envpAddresses[index] !=
                        copiedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            [index]);
                    ++index;
                }
                REQUIRE(index == expectedEnv.size());
            }
        }
        WHEN("We copy assign the given object") {
            Envp assign(
                EnvironmentCollection({{"copy-assign-key1", "copy-assign1"},
                                       {"copy-assign-key2", "copy-assign2"}}));

            // Save the pointers to the arguments
            vector<const char*> envpAddresses;
            const char* const* returnedEnvp = envp.getEnvp();
            size_t index = 0U;
            while(
                returnedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    [index] != nullptr) {
                envpAddresses.push_back(
                    returnedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index]);
                ++index;
            }

            assign = envp;

            THEN("The values of the pointers must be equal") {
                const auto copiedEnvp = assign.getEnvp();
                size_t index = 0U;
                while(
                    copiedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index] !=
                    nullptr) { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    REQUIRE(
                        expectedEnv[index] ==
                        copiedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            [index]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    ++index;
                }
                REQUIRE(index == expectedEnv.size());
            }

            THEN("It must have been a deep copy/the actual pointer must be "
                 "different") {
                const auto copiedEnvp = assign.getEnvp();
                size_t index = 0U;
                while(
                    copiedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index] !=
                    nullptr) { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    REQUIRE(
                        envpAddresses[index] !=
                        copiedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            [index]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    ++index;
                }
                REQUIRE(index == expectedEnv.size());
            }
        }
        WHEN("We move the given object") {
            Envp copied(envp); // Copy so we can move the copy
            Envp moved(move(copied));

            THEN("We must find the expected content") {
                const char* const* movedEnvp = moved.getEnvp();
                size_t index = 0U;
                while(
                    movedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index] != nullptr) {
                    REQUIRE(
                        expectedEnv[index] ==
                        movedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            [index]);
                    ++index;
                }
                REQUIRE(index == expectedEnv.size());
            }
        }
        WHEN("We move assign the given object") {
            Envp copied(envp); // Copy so we can move the copy
            Envp assign(
                EnvironmentCollection({{"move-assign-key1", "move-assign1"},
                                       {"move-assign-key2", "move-assign2"}}));
            assign = move(copied); // NOLINT(hicpp-invalid-access-moved)
            THEN("We must find the expected content") {
                const char* const* movedEnvp = assign.getEnvp();
                size_t index = 0U;
                while(
                    movedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index] != nullptr) {
                    REQUIRE(
                        expectedEnv[index] ==
                        movedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            [index]);
                    ++index;
                }
                REQUIRE(index == expectedEnv.size());
            }
        }
        WHEN("We swap the given object") {
            const EnvironmentCollection swappedContent(
                {{"swap-key1", "swap-value1"},
                 {"swap-key2", "swap-value2"},
                 {"swap-key3", "swap-value3"}});
            vector<string> expectedSwapEnv;
            expectedSwapEnv.reserve(swappedContent.size());
            for(const auto& element : swappedContent) {
                expectedSwapEnv.emplace_back(
                    string(element.first).append("=").append(element.second));
            }
            Envp swapped(swappedContent);
            envp.swap(swapped); // NOLINT(hicpp-invalid-access-moved)
            THEN("We must find the expected content for the first object") {
                const char* const* swappedEnvp = envp.getEnvp();
                size_t index = 0U;
                while(
                    swappedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index] != nullptr) {
                    REQUIRE(
                        swappedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            [index] == expectedSwapEnv[index]);
                    ++index;
                }
                REQUIRE(index == expectedSwapEnv.size());
            }
            THEN("We must find the swapped content for the second object") {
                const char* const* swappedEnvp = swapped.getEnvp();
                size_t index = 0U;
                while(
                    swappedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        [index] != nullptr) {
                    REQUIRE(
                        swappedEnvp // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            [index] == expectedEnv[index]);
                    ++index;
                }
                REQUIRE(index == expectedEnv.size());
            }
        }
    }
}

SCENARIO("Test the envp streaming operator", "[config][envp]") {
    GIVEN("An envp object to stream") {
        const EnvironmentCollection env({{"stream-key1", "stream1"},
                                         {"stream-key2", "stream2"},
                                         {"stream-key3", "stream3"}});
        const Envp envp(env);

        WHEN("We stream the argv object") {
            stringstream stream;
            stream << envp;

            THEN("We must find the expected one") {
                stringstream correctStream;
                bool firstIteration = true;
                for(const auto& envPair : env) {
                    if(!firstIteration) {
                        correctStream << ", ";
                    } else {
                        firstIteration = false;
                    }
                    correctStream << envPair.first << "=" << envPair.second;
                }
                REQUIRE(stream.str() == correctStream.str());
            }
        }
    }
}
} // namespace execHelper::config::test
