#ifndef __MAP_PERMUTATOR_H__
#define __MAP_PERMUTATOR_H__

#include <map>
#include <vector>

namespace execHelper {
    namespace core {
        template<typename T, typename U>
        class MapPermutator {
            public:
                explicit MapPermutator(std::map<T,std::vector<U>> map) noexcept :
                    m_map(std::move(map))
                {
                    ;
                }

                template<typename IteratorType>
                class Iterator {
                    private:
                        using value_type = IteratorType;
                        using pointer = IteratorType*;
                        using reference = IteratorType&;
                        using iterator = Iterator;
 
                    public:
                        explicit Iterator(const std::map<T, std::vector<U>>& map, bool end = false) noexcept :
                            m_map(map),
                            m_end(end)
                        {
                            for(const auto& mapElement : map) {
                                uint32_t index = 0U;
                                if(mapElement.second.empty()) {
                                    m_end = true;
                                }
                                if(m_end) {
                                    index = mapElement.second.size();
                                }
                                m_indexes.emplace(mapElement.first, index);
                            }
                        }

                        iterator& operator++() noexcept {
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

                        bool operator==(const iterator& other) const {
                            if(m_map != other.m_map) {
                                return false;
                            }
                            if(m_end == true && m_end == other.m_end) {
                                return true;
                            }
                            for(const auto& indexElement : m_indexes) {
                                if(indexElement.second != other.m_indexes[indexElement.first]) {
                                    return false;
                                }
                            }
                            return true;
                        }

                        bool operator!=(const iterator& other) const {
                            return !(*this == other);
                        }

                        value_type operator*() const {
                            value_type result;
                            for(const auto& index : m_indexes) {
                                result.insert(std::make_pair(index.first, m_map.at(index.first)[index.second]));
                            }
                            return result;
                        }

                    private:
                        const std::map<T, std::vector<U>>& m_map;
                        mutable std::map<T, uint32_t> m_indexes;
                        bool m_end;
                };

                using iterator = Iterator<std::map<T, U>>;
                using const_iterator = Iterator<const std::map<T, U>>;

                iterator begin() noexcept {
                    return iterator(m_map);
                }

                const_iterator begin() const noexcept {
                    return const_iterator(m_map);
                }

                iterator end() noexcept {
                    return iterator(m_map, true);
                }

                const_iterator end() const noexcept {
                    return const_iterator(m_map, true);
                }

            private:
                const std::map<T, std::vector<U>> m_map;
        };
    } // namespace core
} // namespace execHelper

#endif  /* __MAP_PERMUTATOR_H__ */
