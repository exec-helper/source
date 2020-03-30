#ifndef CONFIG_GENERATORS_INCLUDE
#define CONFIG_GENERATORS_INCLUDE

#include <array>
#include <map>
#include <thread>

#include "config/commandLineOptions.h"
#include "config/pattern.h"
#include "config/settingsNode.h"

#include "unittest/rapidcheck.h"
#include "utils/testValue.h"

// NOTE: Must be in rc namespace!
namespace rc {

struct BoolValue : public execHelper::test::utils::TestValue<bool> {
    BoolValue(bool option) noexcept
        : execHelper::test::utils::TestValue<bool>(option) {}

    std::string config() const noexcept override final {
        return m_value ? "yes" : "no";
    }
};

struct JobsValue
    : public execHelper::test::utils::TestValue<execHelper::config::Jobs_t> {
    JobsValue(execHelper::config::Jobs_t option) noexcept
        : execHelper::test::utils::TestValue<execHelper::config::Jobs_t>(
              option) {}

    std::string config() const noexcept override {
        if(m_value == 0) {
            return "auto";
        }
        return std::to_string(m_value);
    }

    bool operator==(const execHelper::config::Jobs_t& other) const
        noexcept override final {
        if(m_value == 0) {
            return other == std::thread::hardware_concurrency();
        }
        return m_value == other;
    }
};

struct HelpValue : public BoolValue {
    HelpValue(execHelper::config::HelpOption_t option) noexcept
        : BoolValue(option) {}
};

struct VersionValue : public BoolValue {
    VersionValue(bool option) noexcept : BoolValue(option) {}
};

struct VerbosityValue : public BoolValue {
    VerbosityValue(bool option) noexcept : BoolValue(option) {}
};

struct DryRunValue : public BoolValue {
    DryRunValue(bool option) noexcept : BoolValue(option) {}
};

struct ListPluginsValue : public BoolValue {
    ListPluginsValue(bool option) noexcept : BoolValue(option) {}
};

template <> struct Arbitrary<JobsValue> {
    static Gen<JobsValue> arbitrary() {
        return gen::construct<JobsValue>(gen::arbitrary<JobsValue::Value>());
    };
};

template <> struct Arbitrary<HelpValue> {
    static Gen<HelpValue> arbitrary() {
        return gen::construct<HelpValue>(gen::arbitrary<HelpValue::Value>());
    };
};

template <> struct Arbitrary<VersionValue> {
    static Gen<VersionValue> arbitrary() {
        return gen::construct<VersionValue>(
            gen::arbitrary<VersionValue::Value>());
    };
};

template <> struct Arbitrary<VerbosityValue> {
    static Gen<VerbosityValue> arbitrary() {
        return gen::construct<VerbosityValue>(
            gen::arbitrary<VerbosityValue::Value>());
    };
};

template <> struct Arbitrary<DryRunValue> {
    static Gen<DryRunValue> arbitrary() {
        return gen::construct<DryRunValue>(
            gen::arbitrary<DryRunValue::Value>());
    };
};

template <> struct Arbitrary<ListPluginsValue> {
    static Gen<ListPluginsValue> arbitrary() {
        return gen::construct<ListPluginsValue>(
            gen::arbitrary<ListPluginsValue::Value>());
    };
};

// TODO: further extend this. Currently generates settings nodes of exactly 2 levels deep + root level
template <> struct Arbitrary<execHelper::config::SettingsNode> {
    static Gen<execHelper::config::SettingsNode> arbitrary() {
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
    static Gen<execHelper::config::Pattern> arbitrary() {
        return gen::construct<execHelper::config::Pattern>(
            gen::arbitrary<std::string>(),
            gen::arbitrary<std::vector<std::string>>());
    };
};
} // namespace rc

#endif /* CONFIG_GENERATORS_INCLUDE */
