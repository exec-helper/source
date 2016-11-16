#ifndef __PERMUTATION_ITERATOR_H__
#define __PERMUTATION_ITERATOR_H__

#include <iterator>

namespace execHelper {
    namespace core {
        // Currently the PermutationIterator implements std::forward_iterator_tag interface
        template<typename IteratorType, typename CollectionType>
        class CollectionIterator {
            private:
                typedef IteratorType value_type;
                typedef IteratorType* pointer;
                typedef IteratorType& reference;
                typedef CollectionIterator<IteratorType, CollectionType> iterator;

                typename CollectionType::const_iterator m_collectionIterator;
                const typename CollectionType::const_iterator m_collectionBeginIterator;
                const typename CollectionType::const_iterator m_collectionEndIterator;

            public:
                CollectionIterator(const typename CollectionType::const_iterator& beginIterator, const typename CollectionType::const_iterator& endIterator) noexcept :
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
                    return IteratorType(getElementValue());
                }

                typename CollectionType::value_type getElementValue() const {
                    assert(m_collectionIterator != m_collectionEndIterator);
                    return  *m_collectionIterator;
                }

                void restart() {
                    m_collectionIterator = m_collectionBeginIterator;
                }
        };

        template<typename IteratorType, typename CollectionType1, typename CollectionType2>
        class PermutationIterator {
            private:
                typedef IteratorType value_type;
                typedef IteratorType* pointer;
                typedef IteratorType& reference;
                typedef PermutationIterator<IteratorType, CollectionType1, CollectionType2> iterator;

                reference m_description;
                typename CollectionType1::const_iterator m_outerIterator;
                const typename CollectionType1::const_iterator m_outerEndIterator;
                CollectionIterator<IteratorType, CollectionType2> m_innerIterator;

            public:
                PermutationIterator(reference description, const typename CollectionType1::const_iterator& c1BeginIterator, const typename CollectionType1::const_iterator& c1EndIterator, const typename CollectionType2::const_iterator& c2BeginIterator, const typename CollectionType2::const_iterator& c2EndIterator) noexcept :
                    m_description(description),
                    m_outerIterator(c1BeginIterator),
                    m_outerEndIterator(c1EndIterator),
                    m_innerIterator(c2BeginIterator, c2EndIterator)
                {
                    ;
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
                    if(outerAtEnd()) {
                        // If the outer iterator is at the end, we do not care about the state of the inner iterators
                        return m_outerIterator == other.m_outerIterator;
                    }
                    return (m_outerIterator == other.m_outerIterator && m_innerIterator == other.m_innerIterator);
                }

                bool operator!=(const iterator& other) const {
                    return !(*this == other);
                }

                value_type operator*() const {
                    assert(m_outerIterator != m_outerEndIterator);
                    assert(!m_innerIterator.atEnd());
                    return IteratorType({*m_outerIterator}, {m_innerIterator.getElementValue()});
                }

            private:
                bool outerAtEnd() const {
                    return m_outerIterator == m_outerEndIterator;
                }
        };
    }
}

#endif /* __PERMUTATION_ITERATOR_H__ */
