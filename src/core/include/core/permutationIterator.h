#ifndef __PERMUTATION_ITERATOR_H__
#define __PERMUTATION_ITERATOR_H__

#include <iterator>

namespace execHelper {
    namespace core {
        // Currently implements std::forward_iterator_tag interface
        template<typename IteratorType, typename CollectionType1, typename CollectionType2>
        class PermutationIterator : public std::iterator<
                    std::forward_iterator_tag,   // iterator_category
                    IteratorType,                      // value_type
                    IteratorType,                      // difference_type
                    IteratorType*,               // pointer
                    IteratorType&                       // reference
                                  > {
            private:
                typedef IteratorType value_type;
                typedef IteratorType* pointer;
                typedef IteratorType& reference;
                typedef PermutationIterator<IteratorType, CollectionType1, CollectionType2> iterator;

                reference m_description;
                typename CollectionType1::const_iterator m_collection1Iterator;
                typename CollectionType1::const_iterator m_collection1EndIterator;
                typename CollectionType2::const_iterator m_collection2BeginIterator;
                typename CollectionType2::const_iterator m_collection2Iterator;
                typename CollectionType2::const_iterator m_collection2EndIterator;

            public:
                PermutationIterator(reference description, const typename CollectionType1::const_iterator& c1BeginIterator, const typename CollectionType1::const_iterator& c1EndIterator, const typename CollectionType2::const_iterator& c2BeginIterator, const typename CollectionType2::const_iterator& c2EndIterator) noexcept :
                    m_description(description),
                    m_collection1Iterator(c1BeginIterator),
                    m_collection1EndIterator(c1EndIterator),
                    m_collection2BeginIterator(c2BeginIterator),
                    m_collection2Iterator(c2BeginIterator),
                    m_collection2EndIterator(c2EndIterator)
                {
                    ;
                }

                iterator& operator++() noexcept {
                    // The iterator is at its end if the m_collection1Iterator is at its own end
                    if(m_collection1Iterator != m_collection1EndIterator) {
                        if(m_collection2Iterator != m_collection2EndIterator) {
                            ++m_collection2Iterator;
                        }

                        if(m_collection2Iterator == m_collection2EndIterator) {
                            // Means we are at the end of collection2
                            m_collection2Iterator = m_collection2BeginIterator;
                            ++m_collection1Iterator;
                        }
                    }
                    return *this;
                }

                bool operator==(const iterator& other) const {
                    if(m_collection1Iterator == m_collection1EndIterator) {
                        // If the outer iterator is at the end, we do not care about the state of the inner iterators
                        return m_collection1Iterator == other.m_collection1Iterator;
                    }
                    return (m_collection1Iterator == other.m_collection1Iterator && m_collection2Iterator == other.m_collection2Iterator);
                }

                bool operator!=(const iterator& other) const {
                    return !(*this == other);
                }

                value_type operator*() const {
                    assert(m_collection1Iterator != m_collection1EndIterator);
                    assert(m_collection2Iterator != m_collection2EndIterator);
                    return IteratorType({*m_collection1Iterator}, {*m_collection2Iterator});
                }
        };
    }
}

#endif /* __PERMUTATION_ITERATOR_H__ */
