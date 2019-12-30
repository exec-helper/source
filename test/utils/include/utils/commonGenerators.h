#ifndef COMMON_GENERATORS_INCLUDE
#define COMMON_GENERATORS_INCLUDE

#include <filesystem>
#include <optional>

#include "rapidcheck.h"

namespace rc {
template <> struct Arbitrary<std::filesystem::path> {
    static Gen<std::filesystem::path> arbitrary() {
        // TODO: Make this more challenging
        return gen::just(std::filesystem::temp_directory_path());
    }
};

template <typename T> struct Arbitrary<std::optional<T>> {
    static Gen<std::optional<T>> arbitrary() {
        return gen::oneOf(
            gen::construct<std::optional<
                T>>(), // Create optional without content (std::nullopt)
            gen::construct<std::optional<T>>(
                gen::arbitrary<T>()) // Create optional with content
        );
    }
};
} // namespace rc

#endif /* COMMON_GENERATORS_INCLUDE */
