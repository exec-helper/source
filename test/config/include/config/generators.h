#ifndef CONFIG_GENERATORS_INCLUDE
#define CONFIG_GENERATORS_INCLUDE

#include <thread>

#include "config/commandLineOptions.h"

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
} // namespace rc

#endif /* CONFIG_GENERATORS_INCLUDE */
