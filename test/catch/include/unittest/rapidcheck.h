#ifndef UNITTEST_RAPIDCHECK_INCLUDE
#define UNITTEST_RAPIDCHECK_INCLUDE

#include <string>
#include <utility>

#include <rapidcheck.h>

#include "catch.h"

namespace execHelper {
namespace test {
template <typename Testable>
void propertyTest(const std::string& description, Testable&& testable) {
    GIVEN(description) {
        const auto result =
            rc::detail::checkTestable(std::forward<Testable>(testable));

        if(result.template is<rc::detail::SuccessResult>()) {
            const auto success =
                result.template get<rc::detail::SuccessResult>();
            if(!success.distribution.empty()) {
                std::cout << "- " << description << std::endl;
                rc::detail::printResultMessage(result, std::cout);
                std::cout << std::endl;
            }
        } else {
            std::ostringstream ss;
            rc::detail::printResultMessage(result, ss);
            INFO(ss.str() << "\n");
            FAIL();
        }
    }
}
} // namespace test
} // namespace execHelper

#endif /* UNITTEST_RAPIDCHECK_INCLUDE */
