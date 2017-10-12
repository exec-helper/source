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
