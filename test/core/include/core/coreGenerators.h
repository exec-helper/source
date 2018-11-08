#ifndef CORE_GENERATORS_INCLUDE
#define CORE_GENERATORS_INCLUDE

#include <string>
#include <vector>

#include "unittest/rapidcheck.h"

#include "core/task.h"

namespace rc {
template <> struct Arbitrary<execHelper::core::Task> {
    static Gen<execHelper::core::Task> arbitrary() {
        return gen::construct<execHelper::core::Task>(
            gen::arbitrary<std::vector<std::string>>());
    };
};

} // namespace rc

#endif /* CORE_GENERATORS_INCLUDE */
