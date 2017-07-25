#ifndef IS_CONTAINER_INCLUDE
#define IS_CONTAINER_INCLUDE

namespace execHelper {
    namespace config {
        template<class T>
        struct isContainer : public std::false_type {};

        template<class T, class Alloc>
        struct isContainer<std::vector<T, Alloc>> : public std::true_type {};
    } // config
} // execHelper

#endif  /* IS_CONTAINER_INCLUDE */
