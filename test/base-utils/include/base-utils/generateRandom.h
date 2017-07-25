#ifndef GENERATE_RANDOM_INCLUDE
#define GENERATE_RANDOM_INCLUDE

#include <random>
#include <string>

namespace execHelper {
    namespace test {
        namespace baseUtils {
            template<typename T, typename Engine>
            T generateRandomInt(size_t min, size_t max, Engine& engine) noexcept {
                std::uniform_int_distribution<> dis(min, max);
                return uint8_t(dis(engine));
            }

            template<typename T>
            T generateRandomInt(size_t min, size_t max) noexcept {
                static std::mt19937 gen(0);
                return generateRandomInt<T, std::mt19937>(min, max, gen);
            }

            char generateRandomChar() noexcept;
            std::vector<char> generateRandomChar(size_t length) noexcept;
            std::string generateRandomString(size_t length) noexcept; 
        } // namespace baseUtils
    } // namespace test
} // namespace execHelper

#endif  /* GENERATE_RANDOM_INCLUDE */
