#ifndef ASSERT_INCLUDE
#define ASSERT_INCLUDE

#ifdef TERMINATE_ON_ASSERT_FAILURE

#include <iostream>
#include <string>

namespace execHelper {
    namespace log {
        static inline void assertHelper(bool cond, const std::string& message) noexcept {
            if(!cond) {
                std::cerr << message << std::endl;
                std::terminate();
            }
        }
    } // namespace log
} // namespace execHelper

#define assertMessage(cond, prefix, message) do { ::execHelper::log::assertHelper(cond, std::string(__FILE__).append(":").append(std::to_string(__LINE__)).append(":0 ").append(prefix).append(": ").append(message)); } while(false);

/**
 * Checks that an argument does not violate certain conditions (nominal programming style)
 */
#define expectsMessage(cond, message)   assertMessage(cond, "Precondition violated", message);
#define expects(cond) expectsMessage(cond, #cond);

/**
 * Checks that an invariant still holds
 */
#define ensures(cond)   assertMessage(cond, "Invariant violated", #cond);

#else

#define expects(cond)
#define ensures(cond)

#endif

#endif  /* ASSERT_INCLUDE */
