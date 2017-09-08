#ifndef __CREATE_OBJECT_H__
#define __CREATE_OBJECT_H__

#include <utility>

namespace execHelper {
    namespace core {
        namespace detail {
            /**
             * \brief Create an object using the values in the given tuple as the arguments for the constructor of the new object
             */
            template<typename ReturnType, typename Tup, bool end, int Total, int... N>
            struct createObject {
                /**
                 * Create the object
                 *
                 * \param tuple    The tuple containing the arguments for creating the object
                 * \returns The created object
                 */
                static ReturnType getObject(Tup&& tuple) {
                    // Recursively index the list for the number of arguments in N, so that we can use std::get<1,2,3..Total>() on the tuple
                    //   once the whole tuple has been indexed
                    return createObject<ReturnType, Tup, Total ==  1 + sizeof...(N), Total, N..., sizeof...(N)>::getObject(std::forward<Tup>(tuple));
                }
            };
            
            /*! @copydoc createObject
             *
             * Partial template specialization: End condition for the indexing.
             * See \ref createObject for the documentation.
             */
            template<typename ReturnType, typename Tup, int Total, int... N>
            struct createObject<ReturnType, Tup, true, Total, N...> {
                /**
                 * This is a partial template specialization. Please check the generic template declaration for the documentation.
                 *
                 * \param tuple A tuple containing the arguments to pass to the constructor of the ReturnType
                 * \returns A ReturnType object constructed using the given arguments
                 */
                static ReturnType getObject(Tup&& tuple) {
                    // Convert the tuple to a function call with arguments using std::get<1,2,3..Total>()
                    return ReturnType(std::get<N>(std::forward<Tup>(tuple))...);
                }
            };
        }
    }
}

#endif  /* __CREATE_OBJECT_H__ */
