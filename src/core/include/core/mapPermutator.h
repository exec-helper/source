#ifndef __MAP_PERMUTATOR_H__
#define __MAP_PERMUTATOR_H__

#include <map>
#include <vector>
#include <iostream>

namespace execHelper {
    namespace core {
        template<typename T, typename U>
        class MapPermutator {
            public:
                MapPermutator(const std::map<T,std::vector<U>>& map) noexcept :
                    m_map(map)
                {
                    ;
                }

                template<typename IteratorType>
                class Iterator {
                    private:
                        typedef IteratorType value_type;
                        typedef IteratorType* pointer;
                        typedef IteratorType& reference;
                        typedef Iterator iterator;
 
                    public:
                        Iterator(const std::map<T, std::vector<U>>& map, bool end = false) noexcept :
                            m_map(map),
                            m_end(end)
                        {
                            for(const auto& mapElement : map) {
                                uint32_t index = 0U;
                                if(mapElement.second.size() == 0) {
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

                typedef Iterator<std::map<T, U>> iterator;
                typedef Iterator<const std::map<T, U>> const_iterator;

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
    }
}

#endif  /* __MAP_PERMUTATOR_H__ */
