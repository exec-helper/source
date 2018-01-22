#ifndef BASE_UTILS_GENERATORS_INCLUDE
#define BASE_UTILS_GENERATORS_INCLUDE

#include "unittest/rapidcheck.h"

#include "tmpFile.h"

// NOTE: Must be in rc namespace!
namespace rc {

template <> struct Arbitrary<execHelper::test::baseUtils::TmpFile> {
    static Gen<execHelper::test::baseUtils::TmpFile> arbitrary() {
        return gen::construct<execHelper::test::baseUtils::TmpFile>();
    };
};

} // namespace rc

#endif /* BASE_UTILS_GENERATORS_INCLUDE */
