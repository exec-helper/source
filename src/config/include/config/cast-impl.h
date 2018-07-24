#ifndef CAST_IMPL_INCLUDE
#define CAST_IMPL_INCLUDE

#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "log/log.h"

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
    static boost::optional<bool> cast(const U& values) noexcept;
};

/**
 * \brief Partial specialization for casting the given type U to an optional of
 * type vector<T>
 */
template <typename T, typename U> class Cast<std::vector<T>, U> {
  public:
    /*! \copydoc Cast<T,U>::cast(const U& values)
     */
    static boost::optional<std::vector<T>> cast(const U& values) noexcept;
};

/**
 * \brief Partial specialization for casting the given type U to an optional of
 * type Path
 */
template <typename U> class Cast<Path, U> {
  public:
    /*! \copydoc Cast<T,U>::cast(const U& values)
     */
    static boost::optional<Path> cast(const U& values) noexcept;
};

template <typename T, typename U>
inline boost::optional<T> Cast<T, U>::cast(const U& values) noexcept {
    if(values.size() == 0U) {
        return boost::none;
    }
    try {
        return boost::lexical_cast<T>(values.back());
    } catch(boost::bad_lexical_cast& e) {
        user_feedback_error("Internal error");
        return boost::none;
    }
}

template <typename U>
inline boost::optional<bool> Cast<bool, U>::cast(const U& values) noexcept {
    if(values.size() == 0U) {
        return boost::none;
    }
    return (values.back() == "yes" || values.back() == "1" ||
            values.back() == "true");
}

template <typename T, typename U>
inline boost::optional<std::vector<T>>
Cast<std::vector<T>, U>::cast(const U& values) noexcept {
    std::vector<T> result;
    result.reserve(values.size());
    for(const auto& value : values) {
        result.push_back(boost::lexical_cast<T>(value));
    }
    return result;
}

template <typename U>
inline boost::optional<Path> Cast<Path, U>::cast(const U& values) noexcept {
    auto stringValue = Cast<std::string, U>::cast(values);
    if(!stringValue) {
        return boost::none;
    }
    return Path(stringValue.get());
}
} // namespace detail
} // namespace config
} // namespace execHelper

#endif /* CAST_IMPL_INCLUDE */
