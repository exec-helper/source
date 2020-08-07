#ifndef CONFIG_GENERATORS_INCLUDE
#define CONFIG_GENERATORS_INCLUDE

#include <array>
#include <map>
#include <thread>

#include "config/commandLineOptions.h"
#include "config/path.h"
#include "config/pattern.h"
#include "config/settingsNode.h"

#include "unittest/rapidcheck.h"
#include "utils/commonGenerators.h"
#include "utils/testValue.h"

namespace execHelper::config::test {
template <typename T> class Unique {
  public:
    explicit Unique(T value) : m_value{std::move(value)} {}

    const auto& get() const { return m_value; }

    const auto& operator*() const { return get(); }

    const auto* operator->() const { return &m_value; }

  private:
    const T m_value;
};
} // namespace execHelper::config::test

namespace execHelper::config {
inline bool operator<(const Pattern& lhs, const Pattern& rhs) {
    return lhs.getKey() < rhs.getKey();
}
} // namespace execHelper::config

// NOTE: Must be in rc namespace!
namespace rc {

struct BoolValue : public execHelper::test::utils::TestValue<bool> {
    explicit BoolValue(bool option) noexcept
        : execHelper::test::utils::TestValue<bool>(option) {}

    [[nodiscard]] auto config() const noexcept -> std::string final {
        return m_value ? "yes" : "no";
    }
};

struct JobsValue
    : public execHelper::test::utils::TestValue<execHelper::config::Jobs_t> {
    explicit JobsValue(execHelper::config::Jobs_t option) noexcept
        : execHelper::test::utils::TestValue<execHelper::config::Jobs_t>(
              option) {}

    [[nodiscard]] auto config() const noexcept -> std::string override {
        if(m_value == 0) {
            return "auto";
        }
        return std::to_string(m_value);
    }

    [[nodiscard]] auto
    operator==(const execHelper::config::Jobs_t& other) const noexcept
        -> bool final {
        if(m_value == 0) {
            return other == std::thread::hardware_concurrency();
        }
        return m_value == other;
    }
};

struct HelpValue : public BoolValue {
    explicit HelpValue(execHelper::config::HelpOption_t option) noexcept
        : BoolValue(option) {}
};

struct VersionValue : public BoolValue {
    explicit VersionValue(bool option) noexcept : BoolValue(option) {}
};

struct VerbosityValue : public BoolValue {
    explicit VerbosityValue(bool option) noexcept : BoolValue(option) {}
};

struct DryRunValue : public BoolValue {
    explicit DryRunValue(bool option) noexcept : BoolValue(option) {}
};

struct KeepGoingValue : public BoolValue {
    explicit KeepGoingValue(bool option) noexcept : BoolValue(option) {}
};

struct ListPluginsValue : public BoolValue {
    explicit ListPluginsValue(bool option) noexcept : BoolValue(option) {}
};

using AppendSearchPathValue = execHelper::config::Paths;

template <> struct Arbitrary<JobsValue> {
    static auto arbitrary() {
        return gen::construct<JobsValue>(gen::arbitrary<JobsValue::Value>());
    };
};

template <> struct Arbitrary<HelpValue> {
    static auto arbitrary() {
        return gen::construct<HelpValue>(gen::arbitrary<HelpValue::Value>());
    };
};

template <> struct Arbitrary<VersionValue> {
    static auto arbitrary() {
        return gen::construct<VersionValue>(
            gen::arbitrary<VersionValue::Value>());
    };
};

template <> struct Arbitrary<VerbosityValue> {
    static auto arbitrary() {
        return gen::construct<VerbosityValue>(
            gen::arbitrary<VerbosityValue::Value>());
    };
};

template <> struct Arbitrary<DryRunValue> {
    static auto arbitrary() {
        return gen::construct<DryRunValue>(
            gen::arbitrary<DryRunValue::Value>());
    };
};

template <> struct Arbitrary<KeepGoingValue> {
    static auto arbitrary() {
        return gen::construct<KeepGoingValue>(
            gen::arbitrary<KeepGoingValue::Value>());
    };
};

template <> struct Arbitrary<ListPluginsValue> {
    static auto arbitrary() {
        return gen::construct<ListPluginsValue>(
            gen::arbitrary<ListPluginsValue::Value>());
    };
};

// TODO: further extend this. Currently generates settings nodes of exactly 2 levels deep + root level
template <> struct Arbitrary<execHelper::config::SettingsNode> {
    static auto arbitrary() {
        const auto levelSize = *gen::inRange(0U, 1000U);
        return gen::apply(
            [](const std::string& root,
               const std::map<std::string, std::vector<std::string>>& content) {
                execHelper::config::SettingsNode settings(root);
                std::for_each(content.begin(), content.end(),
                              [&settings](const auto& entry) {
                                  if(!settings.add(std::move(entry.first),
                                                   std::move(entry.second))) {
                                      std::cerr << "Failed to add key"
                                                << std::endl;
                                  }
                              });
                return settings;
            },
            gen::arbitrary<std::string>(),
            gen::container<std::map<std::string, std::vector<std::string>>>(
                levelSize, gen::arbitrary<std::string>(),
                gen::arbitrary<std::vector<std::string>>()));
    };
};

template <> struct Arbitrary<execHelper::config::Pattern> {
    static auto arbitrary() {
        auto key = gen::nonEmpty(gen::string<std::string>());
        auto values = gen::container<std::vector<std::string>>(
            gen::nonEmpty(gen::string<std::string>()));
        //auto values = gen::just<std::vector<std::string>>({"Blaat1", "Blaat2"});
        return gen::construct<execHelper::config::Pattern>(key, values);
    };
};

template <typename T> struct Arbitrary<execHelper::config::test::Unique<T>> {
    static auto arbitrary() {
        return gen::construct<execHelper::config::test::Unique<T>>(
            gen::nonEmpty(
                gen::unique<T>(gen::arbitrary<typename T::value_type>())));
    }
};
} // namespace rc

#endif /* CONFIG_GENERATORS_INCLUDE */
