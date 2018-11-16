#ifndef RAPIDCHECK_INCLUDE
#define RAPIDCHECK_INCLUDE

#include <string>
#include <utility>

#include <rapidcheck.h>

namespace execHelper {
namespace test {
template <typename Testable>
void propertyTest(const std::string& description, Testable&& testable) {
    rc::check(description, std::forward<Testable>(testable));
}
} // namespace test
} // namespace execHelper

#endif /* RAPIDCHECK_INCLUDE */
