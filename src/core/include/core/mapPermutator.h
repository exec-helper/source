#ifndef __MAP_PERMUTATOR_H__
#define __MAP_PERMUTATOR_H__

#include <algorithm>
#include <map>
#include <vector>

namespace execHelper::core {
/**
 * \brief Represents all permutations for the values of a given map
 */
template <typename T, typename U> class MapPermutator {
  public:
    /**
     * Constructor
     *
     * \param[in] map   The map to permutate over
     */
    // cppcheck-suppress passedByValue
    explicit MapPermutator(std::map<T, std::vector<U>> map) noexcept
        : m_map(std::move(map)) {
        ;
    }

    /**
     * \brief Iterator object for iterating over all permutations
     */
    template <typename IteratorType> class Iterator {
      private:
        using value_type = IteratorType;
        using pointer = IteratorType*;
        using reference = IteratorType&;
        using iterator = Iterator;

      public:
        /**
         * Constructor for the iterator
         *
         * \param[in] map   The map to permutate and iterate over
         * \param[in] end   Whether the constructed iterator is an end iterator
         */
        explicit Iterator(const std::map<T, std::vector<U>>& map,
                          bool end = false) noexcept
            : m_map(map), m_end(end) {
            for(const auto& mapElement : map) {
                size_t index = 0U;
                if(mapElement.second.empty()) {
                    m_end = true;
                }
                if(m_end) {
                    index = mapElement.second.size();
                }
                m_indexes.emplace(mapElement.first, index);
            }
        }

        /**
         * Postcrement operator
         *
         * \returns The next iterator element
         */
        auto operator++() noexcept -> iterator& {
            if(m_end) {
                return *this;
            }
            auto it = m_indexes.rbegin();
            for(; it != m_indexes.rend(); ++it) {
                ++it->second;
                if(it->second == m_map.at(it->first).size()) {
                    it->second = 0U;
                } else {
                    break;
                }
            }

            // Check if we went through the entire map
            if(it == m_indexes.rend()) {
                // Means we are at the end
                m_end = true;
            }
            return *this;
        }

        /**
         * Equality operator
         *
         * \param[in] other The other object to compare with
         * \returns True    If the objects are considered equal
         *          False   Otherwise
         */
        auto operator==(const iterator& other) const -> bool {
            if(m_map != other.m_map) {
                return false;
            }
            if(m_end && m_end == other.m_end) {
                return true;
            }
            return std::all_of(m_indexes.begin(), m_indexes.end(),
                               [&other](const auto& element) {
                                   return element.second ==
                                          other.m_indexes[element.first];
                               });
        }

        /**
         * Inequality operator
         *
         * \param[in] other The other object to compare with
         * \returns !operator==(other)
         */
        auto operator!=(const iterator& other) const -> bool {
            return !(*this == other);
        }

        /**
         * Dereference operator
         *
         * \returns The dereferenced object
         */
        auto operator*() const -> value_type {
            value_type result;
            for(const auto& index : m_indexes) {
                result.insert(std::make_pair(
                    index.first, m_map.at(index.first)[index.second]));
            }
            return result;
        }

      private:
        const std::map<T, std::vector<U>>& m_map;
        mutable std::map<T, size_t> m_indexes;
        bool m_end;
    };

    using iterator = Iterator<std::map<T, U>>; //!< brief iterator type
    using const_iterator =
        Iterator<const std::map<T, U>>; //!< brief const iterator type

    /**
     * Return iterator to beginning
     *
     * \returns A begin iterator
     */
    auto begin() noexcept -> iterator { return iterator(m_map); }

    /*! @copydoc begin()
     *
     */
    auto begin() const noexcept -> const_iterator {
        return const_iterator(m_map);
    }

    /**
     * Return iterator to end
     *
     * \returns An end iterator
     */
    auto end() noexcept -> iterator { return iterator(m_map, true); }

    /*! @copydoc end()
     *
     */
    auto end() const noexcept -> const_iterator {
        return const_iterator(m_map, true);
    }

  private:
    const std::map<T, std::vector<U>> m_map;
};
} // namespace execHelper::core

#endif /* __MAP_PERMUTATOR_H__ */
