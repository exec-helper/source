#ifndef GENERATORS_INCLUDE
#define GENERATORS_INCLUDE

#include <rapidcheck/rapidcheck.h>

#include "log/logLevel.h"

// NOTE: Must be in rc namespace!
namespace rc {

template <> struct Arbitrary<execHelper::log::LogLevel> {
    static Gen<execHelper::log::LogLevel> arbitrary() {
        return gen::elementOf(execHelper::log::getLogLevels());
    };
};

} // namespace rc

#endif /* GENERATORS_INCLUDE */
