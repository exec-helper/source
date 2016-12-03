#ifndef __CREATE_OBJECT_H__
#define __CREATE_OBJECT_H__

#include <utility>

namespace execHelper {
    namespace core {
        namespace detail {
            template<typename ReturnType, typename Tup, bool end, int Total, int... N>
            struct createObject {
                static ReturnType getObject(Tup&& tuple) {
                    return createObject<ReturnType, Tup, Total ==  1 + sizeof...(N), Total, N..., sizeof...(N)>::getObject(std::forward<Tup>(tuple));
                }
            };
            
            template<typename ReturnType, typename Tup, int Total, int... N>
            struct createObject<ReturnType, Tup, true, Total, N...> {
                static ReturnType getObject(Tup&& tuple) {
                    return ReturnType(std::get<N>(std::forward<Tup>(tuple))...);
                }
            };
        }
    }
}

#endif  /* __CREATE_OBJECT_H__ */
