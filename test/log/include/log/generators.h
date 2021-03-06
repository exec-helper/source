#ifndef LOG_GENERATORS_INCLUDE
#define LOG_GENERATORS_INCLUDE

#include "log/logLevel.h"

#include "rapidcheck.h"

// NOTE: Must be in rc namespace!
namespace rc {

template <> struct Arbitrary<execHelper::log::LogLevel> {
    static Gen<execHelper::log::LogLevel> arbitrary() {
        return gen::elementOf(execHelper::log::getLogLevels());
    };
};

} // namespace rc

#endif /* LOG_GENERATORS_INCLUDE */
