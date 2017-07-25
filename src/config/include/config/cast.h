#ifndef CAST_INCLUDE
#define CAST_INCLUDE

#include <boost/optional/optional.hpp>

namespace execHelper {
    namespace config {
        namespace detail {
            /**
             * \brief Cast the given type U to an optional of type T
             */
            template<typename T, typename U>
            class Cast {
                public:
                    /**
                     * Cast the given values to type T
                     *
                     * \param[in] values    The values to cast from
                     * \returns T if the values could be casted
                     *          boost::none otherwise
                     */
                    static boost::optional<T> cast(const U& values) noexcept;
            };
        } // namespace detail
    } // namespace config
} // namespace execHelper

#endif  /* CAST_INCLUDE */
