#ifndef LOG_GENERATORS_INCLUDE
#define LOG_GENERATORS_INCLUDE

#include "core/task.h"
#include "log/logLevel.h"

#include "rapidcheck.h"

// NOTE: Must be in rc namespace!
namespace rc {

template <> struct Arbitrary<execHelper::log::LogLevel> {
    static Gen<execHelper::log::LogLevel> arbitrary() {
        return gen::elementOf(execHelper::log::getLogLevels());
    }
};

template <> struct Arbitrary<std::filesystem::path> {
    static Gen<std::filesystem::path> arbitrary() {
        // TODO: Make this more challenging
        return gen::just(std::filesystem::temp_directory_path());
    }
};

template <> struct Arbitrary<execHelper::core::Task> {
    static Gen<execHelper::core::Task> arbitrary() {
        return gen::construct<execHelper::core::Task>(
            gen::arbitrary<execHelper::core::TaskCollection>(),
            gen::arbitrary<execHelper::config::EnvironmentCollection>(),
            gen::arbitrary<execHelper::config::Path>());
    }
};

} // namespace rc

#endif /* LOG_GENERATORS_INCLUDE */
