#ifndef CAST_IMPL_INCLUDE
#define CAST_IMPL_INCLUDE

#include <optional>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "cast.h"
#include "path.h"

namespace execHelper {
namespace config {
namespace detail {
/**
 * \brief Partial specialization for casting the given type U to an optional of
 * type bool
 */
template <typename U> class Cast<bool, U> {
  public:
    /*! @copydoc Cast<T,U>::cast(const U& values)
     */
    static std::optional<bool> cast(const U& values) noexcept;
};

/**
 * \brief Partial specialization for casting the given type U to an optional of
 * type vector<T>
 */
template <typename T, typename U> class Cast<std::vector<T>, U> {
  public:
    /*! \copydoc Cast<T,U>::cast(const U& values)
     */
    static std::optional<std::vector<T>> cast(const U& values) noexcept;
};

/**
 * \brief Partial specialization for casting the given type U to an optional of
 * type Path
 */
template <typename U> class Cast<Path, U> {
  public:
    /*! \copydoc Cast<T,U>::cast(const U& values)
     */
    static std::optional<Path> cast(const U& values) noexcept;
};

template <typename T, typename U>
inline std::optional<T> Cast<T, U>::cast(const U& values) noexcept {
    if(values.size() == 0U) {
        return std::nullopt;
    }
    try {
        return std::make_optional(boost::lexical_cast<T>(values.back()));
    } catch(boost::bad_lexical_cast&) {
        return std::nullopt;
    }
}

template <typename U>
inline std::optional<bool> Cast<bool, U>::cast(const U& values) noexcept {
    if(values.size() == 0U) {
        return std::nullopt;
    }
    return (values.back() == "yes" || values.back() == "1" ||
            values.back() == "true");
}

template <typename T, typename U>
inline std::optional<std::vector<T>>
Cast<std::vector<T>, U>::cast(const U& values) noexcept {
    std::vector<T> result;
    result.reserve(values.size());
    for(const auto& value : values) {
        result.push_back(boost::lexical_cast<T>(value));
    }
    return result;
}

template <typename U>
inline std::optional<Path> Cast<Path, U>::cast(const U& values) noexcept {
    auto stringValue = Cast<std::string, U>::cast(values);
    if(!stringValue) {
        return std::nullopt;
    }
    return Path(stringValue.value());
}
} // namespace detail
} // namespace config
} // namespace execHelper

#endif /* CAST_IMPL_INCLUDE */
