#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <gsl/gsl>

#include "config/envp.h"
#include "unittest/catch.h"

using gsl::czstring;

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
