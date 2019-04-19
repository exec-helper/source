#ifndef NON_EMPTY_STRING_INCLUDE
#define NON_EMPTY_STRING_INCLUDE

#include <string>

#include "unittest/rapidcheck.h"

namespace execHelper {
namespace test {
/**
 * \brief Generates string that contain only graphically representable characters for your locale
 */
class NonEmptyString {
  public:
    NonEmptyString(const std::string& string) : m_string(string) { ; }

    const std::string& operator*() const noexcept { return m_string; }

    bool operator==(const NonEmptyString& other) const noexcept {
        return m_string == other.m_string;
    }

    bool operator!=(const NonEmptyString& other) const noexcept {
        return !(*this == other);
    }

    bool operator<(const NonEmptyString& other) const noexcept {
        return m_string < other.m_string;
    }

  private:
    std::string m_string;
};

std::ostream& operator<<(std::ostream& os, const NonEmptyString& obj) noexcept {
    os << *obj;
    return os;
}
} // namespace test
} // namespace execHelper

namespace rc {
template <> struct Arbitrary<execHelper::test::NonEmptyString> {
    static Gen<execHelper::test::NonEmptyString> arbitrary() {
        return gen::construct<execHelper::test::NonEmptyString>(
            gen::nonEmpty(gen::string<std::string>()));
    };
};
} // namespace rc

#endif /* NON_EMPTY_STRING_INCLUDE */
