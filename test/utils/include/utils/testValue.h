#ifndef TEST_VALUE_INCLUDE
#define TEST_VALUE_INCLUDE

#include <string>

namespace execHelper::test::utils {
template <typename T> class TestValue {
  public:
    using Value = T;

    TestValue(T value) noexcept : m_value(value) { ; }

    TestValue(T&& value) noexcept : m_value(std::move(value)) { ; }

    inline T& operator*() noexcept { return m_value; }

    inline const T& operator*() const noexcept { return m_value; }

    virtual bool operator==(const T& other) const noexcept {
        return m_value == other;
    }

    inline bool operator!=(const T& other) const noexcept {
        return !(*this == other);
    }

    virtual std::string config() const noexcept = 0;

  protected:
    T m_value;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const TestValue<T>& value) noexcept {
    os << *value;
    return os;
}
} // namespace execHelper::test::utils

#endif /* TEST_VALUE_INCLUDE */
