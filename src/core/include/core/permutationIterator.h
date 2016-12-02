#ifndef __PERMUTATION_ITERATOR_H__
#define __PERMUTATION_ITERATOR_H__

#include <cassert>
#include <tuple>

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

        // Currently the PermutationIterator implements std::forward_iterator_tag interface
        template<typename IteratorType, typename CollectionType1, typename... CollectionTypes>
        class PermutationIterator {
            private:
                typedef IteratorType value_type;
                typedef IteratorType* pointer;
                typedef IteratorType& reference;
                typedef PermutationIterator<IteratorType, CollectionType1, CollectionTypes...> iterator;
                typedef std::tuple<typename CollectionType1::value_type, typename CollectionTypes::value_type...> Tuple;

                const typename CollectionType1::const_iterator m_outerBeginIterator;
                typename CollectionType1::const_iterator m_outerIterator;
                const typename CollectionType1::const_iterator m_outerEndIterator;
                PermutationIterator<IteratorType, CollectionTypes...> m_innerIterator;

            public:
                PermutationIterator(const typename CollectionType1::const_iterator& c1BeginIterator, const typename CollectionTypes::const_iterator&... otherBeginIterators, const typename CollectionType1::const_iterator& c1EndIterator, const typename CollectionTypes::const_iterator&... otherEndIterators) noexcept :
                    m_outerBeginIterator(c1BeginIterator),
                    m_outerIterator(c1BeginIterator),
                    m_outerEndIterator(c1EndIterator),
                    m_innerIterator(otherBeginIterators..., otherEndIterators...)
                {
                    //if(c1BeginIterator == c1EndIterator) {
                        //// Means the collection is empty
                        //m_outerIterator = m_outerEndIterator;  
                    //}
                }

                iterator& operator++() noexcept {
                    // The iterator is at its end if the m_outerIterator is at its own end
                    if(m_outerIterator != m_outerEndIterator) {
                        if(! m_innerIterator.atEnd()) {
                            ++m_innerIterator;
                        }

                        if(m_innerIterator.atEnd()) {
                            m_innerIterator.restart();
                            ++m_outerIterator;
                        }
                    }
                    return *this;
                }

                bool operator==(const iterator& other) const {
                    if(atEnd()) {
                        // If the outer iterator is at the end, we do not care about the state of the inner iterators
                        return m_outerIterator == other.m_outerIterator;
                    }
                    return (m_innerIterator == other.m_innerIterator && m_outerIterator == other.m_outerIterator);
                }

                bool operator!=(const iterator& other) const {
                    return !(*this == other);
                }

                Tuple getElementValue() const {
                    assert(m_outerIterator != m_outerEndIterator);
                    return std::tuple_cat(std::make_tuple(*m_outerIterator), m_innerIterator.getElementValue());
                }

                value_type operator*() const {
                    assert(m_outerIterator != m_outerEndIterator);
                    assert(!m_innerIterator.atEnd());
                    typedef typename std::decay<Tuple>::type ttype;
                    return detail::createObject<IteratorType, Tuple, 0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::getObject(getElementValue());
//                    return IteratorType(*m_outerIterator, std::get<CollectionTypes::value_type...>(std::forward<Tuple>(m_innerIterator.getElementValue()))...);
//                    return IteratorType(*m_outerIterator, std::get<1,2,...,N>(std::forward<Tuple>(m_innerIterator.getElementValue()))...);
                }

                bool atEnd() const {
                    return m_outerIterator == m_outerEndIterator;
                }

                void restart() {
                    m_outerIterator = m_outerBeginIterator;
                    m_innerIterator.restart();
                }
        };

        template<typename IteratorType, typename CollectionType>
        class PermutationIterator<IteratorType, CollectionType> {
            private:
                typedef IteratorType value_type;
                typedef IteratorType* pointer;
                typedef IteratorType& reference;
                typedef PermutationIterator<IteratorType, CollectionType> iterator;

                typename CollectionType::const_iterator m_collectionIterator;
                const typename CollectionType::const_iterator m_collectionBeginIterator;
                const typename CollectionType::const_iterator m_collectionEndIterator;

            public:
                PermutationIterator(const typename CollectionType::const_iterator& beginIterator, const typename CollectionType::const_iterator& endIterator) noexcept :
                    m_collectionIterator(beginIterator),
                    m_collectionBeginIterator(beginIterator),
                    m_collectionEndIterator(endIterator)
                {
                    ;
                }

                iterator& operator++() noexcept {
                    // The iterator is at its end if the m_outerIterator is at its own end
                    if(! atEnd()) {
                        ++m_collectionIterator;
                    }
                    return *this;
                }

                bool atEnd() const {
                    return m_collectionIterator == m_collectionEndIterator;
                }

                bool operator==(const iterator& other) const {
                    return m_collectionIterator == other.m_collectionIterator;
                }

                bool operator!=(const iterator& other) const {
                    return !(*this == other);
                }

                value_type operator*() const {
                    return IteratorType(std::get<CollectionType::value>(getElementValue()));
                }

                std::tuple<typename CollectionType::value_type> getElementValue() const {
                    assert(m_collectionIterator != m_collectionEndIterator);
                    return  std::make_tuple(*m_collectionIterator);
                }

                void restart() {
                    m_collectionIterator = m_collectionBeginIterator;
                }
        };
    }
}

#endif /* __PERMUTATION_ITERATOR_H__ */
