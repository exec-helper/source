#ifndef __PERMUTATION_ITERATOR_H__
#define __PERMUTATION_ITERATOR_H__

#include <tuple>

#include <log/assertions.h>

#include "createObject.h"

namespace execHelper {
namespace core {
/**
 * \brief Iterates over permutations of the given collection
 *
 * Currently the PermutationIterator implements std::forward_iterator_tag
 * interface
 */
template <typename IteratorType, typename CollectionType1,
          typename... CollectionTypes>
class PermutationIterator {
  private:
    using value_type = IteratorType;
    using pointer = IteratorType*;
    using reference = IteratorType&;
    using iterator =
        PermutationIterator<IteratorType, CollectionType1, CollectionTypes...>;
    using Tuple = std::tuple<typename CollectionType1::value_type,
                             typename CollectionTypes::value_type...>;

    const typename CollectionType1::const_iterator m_outerBeginIterator;
    typename CollectionType1::const_iterator m_outerIterator;
    const typename CollectionType1::const_iterator m_outerEndIterator;
    PermutationIterator<IteratorType, CollectionTypes...> m_innerIterator;

  public:
    /**
     * Constructor
     *
     * \param[in] c1BeginIterator   The begin iterator for the first collection
     * \param[in] otherBeginIterators   The begin iterators for additional
     * collections \param[in] c1EndIterator     The end iterator for the first
     * collection \param[in] otherEndIterators The end iterators for additional
     * collections
     */
    PermutationIterator(
        const typename CollectionType1::const_iterator& c1BeginIterator,
        const typename CollectionTypes::const_iterator&... otherBeginIterators,
        const typename CollectionType1::const_iterator& c1EndIterator,
        const typename CollectionTypes::
            const_iterator&... otherEndIterators) noexcept
        : m_outerBeginIterator(c1BeginIterator),
          m_outerIterator(c1BeginIterator),
          m_outerEndIterator(c1EndIterator),
          m_innerIterator(otherBeginIterators..., otherEndIterators...) {
        if(hasEmptyCollection()) {
            m_outerIterator = m_outerEndIterator;
        }
    }

    /**
     * Increment operator
     *
     * \returns An iterator pointing to the next object
     */
    iterator& operator++() noexcept {
        // The iterator is at its end if the m_outerIterator is at its own end
        if(m_outerIterator != m_outerEndIterator) {
            if(!m_innerIterator.atEnd()) {
                ++m_innerIterator;
            }

            if(m_innerIterator.atEnd()) {
                m_innerIterator.restart();
                ++m_outerIterator;
            }
        }
        return *this;
    }

    /**
     * Returns whether any of the given collections is empty
     *
     * \returns True    If any of the given collections is empty
     *          False   Otherwise
     */
    bool hasEmptyCollection() const noexcept {
        return m_outerBeginIterator == m_outerEndIterator ||
               m_innerIterator.hasEmptyCollection();
    }

    /*! @copydoc config::Argv::operator==(const Argv&) const
     */
    bool operator==(const iterator& other) const {
        if(atEnd()) {
            // If the outer iterator is at the end, we do not care about the
            // state of the inner iterators
            return m_outerIterator == other.m_outerIterator;
        }
        return (m_innerIterator == other.m_innerIterator &&
                m_outerIterator == other.m_outerIterator);
    }

    /*! @copydoc config::Argv::operator!=(const Argv&) const
     */
    bool operator!=(const iterator& other) const { return !(*this == other); }

    /**
     * Convert the current iterator position to a tuple of the actual objects
     * they are pointing to
     *
     * \returns The actual objects the current iterator position is pointing to
     */
    Tuple getElementValue() const {
        expectsMessage(m_outerIterator != m_outerEndIterator,
                       "Do not dereference end iterator");
        return std::tuple_cat(std::make_tuple(*m_outerIterator),
                              m_innerIterator.getElementValue());
    }

    /**
     * Dereference operator
     *
     * \returns The object the current iterator is pointing to
     */
    value_type operator*() const {
        expectsMessage(m_outerIterator != m_outerEndIterator,
                       "Do not dereference outer end iterator");
        expectsMessage(!m_innerIterator.atEnd(),
                       "Do not dereference inner end iterator");
        using ttype = typename std::decay<Tuple>::type;
        return detail::createObject<
            IteratorType, Tuple, 0 == std::tuple_size<ttype>::value,
            std::tuple_size<ttype>::value>::getObject(getElementValue());
    }

    /**
     * Returns whether the current iterator is at the end
     *
     * \returns True    If the current iterator is at the end
     *          False   Otherwise
     */
    bool atEnd() const { return m_outerIterator == m_outerEndIterator; }

    /**
     * Restart the iterator
     */
    void restart() {
        m_outerIterator = m_outerBeginIterator;
        m_innerIterator.restart();
    }
};

/*!@copydoc PermutationIterator
 */
template <typename IteratorType, typename CollectionType>
class PermutationIterator<IteratorType, CollectionType> {
  private:
    using value_type = IteratorType;
    using pointer = IteratorType*;
    using reference = IteratorType&;
    using iterator = PermutationIterator<IteratorType, CollectionType>;

    typename CollectionType::const_iterator m_collectionIterator;
    const typename CollectionType::const_iterator m_collectionBeginIterator;
    const typename CollectionType::const_iterator m_collectionEndIterator;

  public:
    /**
     * Constructor
     *
     * \param[in] beginIterator The begin iterator for the collection
     * \param[in] endIterator   The end iterator for the collection
     */
    PermutationIterator(
        const typename CollectionType::const_iterator& beginIterator,
        const typename CollectionType::const_iterator& endIterator) noexcept
        : m_collectionIterator(beginIterator),
          m_collectionBeginIterator(beginIterator),
          m_collectionEndIterator(endIterator) {
        ;
    }

    /*! @copydoc PermutationIterator::operator++()
     */
    iterator& operator++() noexcept {
        // The iterator is at its end if the m_outerIterator is at its own end
        if(!atEnd()) {
            ++m_collectionIterator;
        }
        return *this;
    }

    /*! @copydoc PermutationIterator::atEnd()
     */
    bool atEnd() const {
        return m_collectionIterator == m_collectionEndIterator;
    }

    /*! @copydoc PermutationIterator::hasEmptyCollection()
     */
    bool hasEmptyCollection() const noexcept {
        return m_collectionBeginIterator == m_collectionEndIterator;
    }

    /*! @copydoc PermutationIterator::operator==()
     */
    bool operator==(const iterator& other) const {
        return m_collectionIterator == other.m_collectionIterator;
    }

    /*! @copydoc PermutationIterator::operator!=()
     */
    bool operator!=(const iterator& other) const { return !(*this == other); }

    /*! @copydoc PermutationIterator::operator*()
     */
    value_type operator*() const {
        return IteratorType(std::get<CollectionType::value>(getElementValue()));
    }

    /*! @copydoc PermutationIterator::getElementValue()
     */
    std::tuple<typename CollectionType::value_type> getElementValue() const {
        expectsMessage(m_collectionIterator != m_collectionEndIterator,
                       "Do not dereference end iterator");
        return std::make_tuple(*m_collectionIterator);
    }

    /*! @copydoc PermutationIterator::restart()
     */
    void restart() { m_collectionIterator = m_collectionBeginIterator; }
};
} // namespace core
} // namespace execHelper

#endif /* __PERMUTATION_ITERATOR_H__ */
