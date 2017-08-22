#ifndef __PERMUTATION_ITERATOR_H__
#define __PERMUTATION_ITERATOR_H__

#include <tuple>

#include <gsl/gsl>

#include "createObject.h"

namespace execHelper {
    namespace core {
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
                    if(hasEmptyCollection()) {
                        m_outerIterator = m_outerEndIterator;
                    }
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

                bool hasEmptyCollection() const noexcept {
                    return m_outerBeginIterator == m_outerEndIterator || m_innerIterator.hasEmptyCollection();
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
                    Expects(m_outerIterator != m_outerEndIterator);
                    return std::tuple_cat(std::make_tuple(*m_outerIterator), m_innerIterator.getElementValue());
                }

                value_type operator*() const {
                    Expects(m_outerIterator != m_outerEndIterator);
                    Expects(!m_innerIterator.atEnd());
                    typedef typename std::decay<Tuple>::type ttype;
                    return detail::createObject<IteratorType, Tuple, 0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::getObject(getElementValue());
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

                bool hasEmptyCollection() const noexcept {
                    return m_collectionBeginIterator == m_collectionEndIterator;
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
                    Expects(m_collectionIterator != m_collectionEndIterator);
                    return  std::make_tuple(*m_collectionIterator);
                }

                void restart() {
                    m_collectionIterator = m_collectionBeginIterator;
                }
        };
    }
}

#endif /* __PERMUTATION_ITERATOR_H__ */
