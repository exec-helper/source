#include "generateRandom.h"

#include <algorithm>
#include <cassert>
#include <limits>

using std::generate_n;
using std::numeric_limits;
using std::string;
using std::vector;

namespace  {
    auto NB_OF_VALID_RANDOM_CHARS = 62U;

    inline char convertToAlphaNumeric(uint8_t index) {
        if(index < 10U) {
            return index + '0';
        }
        index -= 10U;
        if(index < 26U) {
            return index + 'a';
        }
        index -= 26U;
        return index + 'A';
    }
} // namespace

namespace execHelper {
    namespace test {
        namespace baseUtils {
            char generateRandomChar() noexcept {
                return convertToAlphaNumeric(generateRandomInt<uint8_t>(0, NB_OF_VALID_RANDOM_CHARS - 1U));
            }

            vector<char> generateRandomChar(size_t length) noexcept {
                vector<char> result;

                auto strengthReductionLevel = 4U;
                using StrengthReducedType = uint8_t;
                assert(strengthReductionLevel * NB_OF_VALID_RANDOM_CHARS < numeric_limits<StrengthReducedType>::max());  // Make sure the resulting value fits the strength reduced type

                while(length >= strengthReductionLevel) {
                    auto randomValue = generateRandomInt<StrengthReducedType>(0, strengthReductionLevel * NB_OF_VALID_RANDOM_CHARS - 1U);
                    result.emplace_back(convertToAlphaNumeric(randomValue));
                    result.emplace_back(convertToAlphaNumeric(randomValue - 1 * NB_OF_VALID_RANDOM_CHARS));
                    result.emplace_back(convertToAlphaNumeric(randomValue - 2 * NB_OF_VALID_RANDOM_CHARS));
                    result.emplace_back(convertToAlphaNumeric(randomValue - 3 * NB_OF_VALID_RANDOM_CHARS));
                    length -= strengthReductionLevel;
                }
                for(size_t i = 0U; i < length; ++i) {
                    result.emplace_back(generateRandomChar()); 
                }
                return result;
            }

            string generateRandomString(size_t length) noexcept {
                auto randomChars = generateRandomChar(length);
                return string(randomChars.begin(), randomChars.end());
            }
        } // namespace baseUtils
    } // namespace test
} // namespace execHelper
