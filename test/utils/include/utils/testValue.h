#ifndef TEST_VALUE_INCLUDE
#define TEST_VALUE_INCLUDE

#include <string>

namespace execHelper::test::utils {
template <typename T> class TestValue {
  public:
    using Value = T;

    explicit TestValue(T value) noexcept : m_value(value) { ; }

    explicit TestValue(T&& value) noexcept : m_value(std::move(value)) { ; }

    virtual ~TestValue() = default;

    inline auto operator*() noexcept -> T& { return m_value; }

    inline auto operator*() const noexcept -> const T& { return m_value; }

    [[nodiscard]]
    virtual auto operator==(const T& other) const noexcept -> bool {
        return m_value == other;
    }

    [[nodiscard]]
    inline auto operator!=(const T& other) const noexcept -> bool {
        return !(*this == other);
    }

    [[nodiscard]]
    virtual auto config() const noexcept -> std::string = 0;

  protected:
    TestValue() = default;
    TestValue(const TestValue& other) = default;
    TestValue(TestValue&& other) noexcept = default;

    auto operator=(const TestValue& other) -> TestValue& = default;
    auto operator=(TestValue&& other) noexcept -> TestValue& = default;

    T m_value;
};

template <typename T>
auto operator<<(std::ostream& os, const TestValue<T>& value) noexcept -> std::ostream& {
    os << *value;
    return os;
}
} // namespace execHelper::test::utils

#endif /* TEST_VALUE_INCLUDE */
