#include <vector>
#include <deque>
#include <string>

#include <catch.hpp>

#include "core/permutationIterator.h"

using std::vector;
using std::deque;
using std::string;

using execHelper::core::PermutationIterator;

namespace {
    class PermuteObjectElement {
        public:
            using Object1 = string;
            using Object2 = uint8_t;
            PermuteObjectElement(Object1 object1, Object2 object2) :
                m_object1(std::move(object1)),
                m_object2(object2)
            {
                ;
            }

            bool operator==(const PermuteObjectElement& other) const noexcept {
                return m_object1 == other.m_object1 && m_object2 == other.m_object2;
            }

            bool operator!=(const PermuteObjectElement& other) const noexcept {
                return !(*this == other);
            }

        private:
            const Object1 m_object1;
            const Object2 m_object2;
    };

    class PermuteObject {
        public:
            using Collection1 = vector<PermuteObjectElement::Object1>;
            using Collection2 =  deque<PermuteObjectElement::Object2>;

            using iterator = PermutationIterator<PermuteObjectElement, Collection1, Collection2>;
            using const_iterator = PermutationIterator<const PermuteObjectElement, const Collection1, const Collection2>;

            PermuteObject(Collection1 collection1, Collection2 collection2) noexcept :
                m_collection1(std::move(collection1)),
                m_collection2(std::move(collection2))
            {
                ;
            }

            bool operator==(const PermuteObject& other) const noexcept {
                return m_collection1 == other.m_collection1 && m_collection2 == other.m_collection2;
            }

            bool operator!=(const PermuteObject& other) const noexcept {
                return !(*this == other);
            }

            const Collection1& getCollection1() const noexcept {
                return m_collection1;
            }

            const Collection2& getCollection2() const noexcept {
                return m_collection2;
            }

            iterator begin() noexcept {
                return iterator(m_collection1.begin(), m_collection2.begin(), m_collection1.end(), m_collection2.end());
            }

            const_iterator begin() const noexcept {
                return const_iterator(m_collection1.begin(), m_collection2.begin(), m_collection1.end(), m_collection2.end());
            }

            iterator end() noexcept {
                return iterator(m_collection1.end(), m_collection2.end(), m_collection1.end(), m_collection2.end());
            }

            const_iterator end() const noexcept {
                return const_iterator(m_collection1.end(), m_collection2.end(), m_collection1.end(), m_collection2.end());
            }

        private:
            Collection1 m_collection1;
            Collection2 m_collection2;

    };

    vector<PermuteObjectElement> getOrderedCombinations(const PermuteObject& permute) {
        vector<PermuteObjectElement> orderedCombinations;
        for(const auto& object1 : permute.getCollection1()) {
            for(const auto& object2 : permute.getCollection2()) {
                PermuteObjectElement combination(object1, object2);
                orderedCombinations.push_back(combination);
            }
        }
        return orderedCombinations;
    }

    template<typename T>
    inline void testForeach(T& permute, const vector<PermuteObjectElement>& orderedCombinations) {
        size_t orderedCombinationsIndex = 0U;
        for(auto combination : permute) {
            REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
            REQUIRE((combination == orderedCombinations[orderedCombinationsIndex]));
            ++orderedCombinationsIndex;
        }
        REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
    }
}

namespace execHelper { namespace core { namespace test {
    SCENARIO("Test the permutation iterators when looping over the entire collection", "[permutationiterator]") {
        GIVEN("Some non-const collections to iterate over using permutations of its content and the ordered combinations") {
            PermuteObject permute({"object1", "object2"}, {1, 2});
            const vector<PermuteObjectElement> orderedCombinations = getOrderedCombinations(permute);

            WHEN("We iterate over them entirely") {
                THEN("We should be able to do so using a foreach loop") {
                    testForeach(permute, orderedCombinations);
                }
                THEN("We should be able to do so using iterators") {
                    size_t orderedCombinationsIndex = 0U;
                    for(auto it = permute.begin(); it != permute.end(); ++it) {
                        REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                        REQUIRE(*it == orderedCombinations[orderedCombinationsIndex]);
                        ++orderedCombinationsIndex;
                    }
                    REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                }
            }
        }

        GIVEN("Some const collections to iterate over using permutations of its content and the ordered combinations") {
            const PermuteObject permute({"object1", "object2"}, {1, 2});
            const vector<PermuteObjectElement> orderedCombinations = getOrderedCombinations(permute);

            WHEN("We iterate over them entirely") {
                THEN("We should be able to do so using a foreach loop") {
                    size_t orderedCombinationsIndex = 0U;
                    for(const auto& combination : permute) {
                        REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                        REQUIRE(combination == orderedCombinations[orderedCombinationsIndex]);
                        ++orderedCombinationsIndex;
                    }
                    REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                }
                THEN("We should be able to do so using iterators") {
                    size_t orderedCombinationsIndex = 0U;
                    for(auto it = permute.begin(); it != permute.end(); ++it) {
                        REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                        REQUIRE(*it == orderedCombinations[orderedCombinationsIndex]);
                        ++orderedCombinationsIndex;
                    }
                    REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                }
            }
        }

        GIVEN("Some collections that have empty collection on certain places") {
            WHEN("Trying to iterate over an empty inner iterator") {
                PermuteObject permute({"object1", "object2"}, {});
                const PermuteObject constPermute({"object1", "object2"}, {});

                THEN("We should get no elements to iterate over") {
                    REQUIRE(permute.begin() == permute.end());
                    REQUIRE(constPermute.begin() == constPermute.end());
                }
            }
            WHEN("Trying to iterate over an empty outer iterator") {
                PermuteObject permute({}, {1, 2});
                const PermuteObject constPermute({"object1", "object2"}, {});

                THEN("We should get no elements to iterate over") {
                    REQUIRE(permute.begin() == permute.end());
                    REQUIRE(constPermute.begin() == constPermute.end());
                }
            }

        }
    }
    SCENARIO("Test the permutation iterators for partial iteration", "[permutationiterator]") {
        GIVEN("Some non-const collections to partially iterate over using permutations of its content and the ordered combinations") {
            PermuteObject permute({"object1", "object2", "object3", "object4"}, {1, 2, 3, 4, 5, 6, 7, 8});

            vector<PermuteObjectElement> orderedCombinations;
            const size_t beginIndexObject1 = 1U;
            const size_t endIndexObject1 = permute.getCollection1().size() - 1U;
            const size_t beginIndexObject2 = 1U;
            const size_t endIndexObject2 = permute.getCollection2().size() - 1U;

            for(size_t i = beginIndexObject1; i < endIndexObject1; ++i) {
                for(size_t j = beginIndexObject2; j < endIndexObject2; ++j) {
                    PermuteObjectElement combination(permute.getCollection1()[i], permute.getCollection2()[j]);
                    orderedCombinations.push_back(combination);
                }
            }

            WHEN("We iterate over them partially") {
                THEN("We should get the same ordered combinations") {
                    size_t orderedCombinationsIndex = 0U;
                    for(auto it = PermutationIterator<PermuteObjectElement, PermuteObject::Collection1, PermuteObject::Collection2>(permute.getCollection1().begin() + beginIndexObject1, permute.getCollection2().begin() + beginIndexObject2, permute.getCollection1().begin() + endIndexObject1, permute.getCollection2().begin() + endIndexObject2);
                            it != PermutationIterator<PermuteObjectElement, PermuteObject::Collection1, PermuteObject::Collection2>(permute.getCollection1().begin() + endIndexObject1, permute.getCollection2().begin() + endIndexObject2, permute.getCollection1().begin() + endIndexObject1, permute.getCollection2().begin() + endIndexObject2);
                            ++it) {
                        REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                        REQUIRE(*it == orderedCombinations[orderedCombinationsIndex]);
                        ++orderedCombinationsIndex;
                    }
                    REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                }
            }
        }
        GIVEN("Some const collections to partially iterate over using permutations of its content and the ordered combinations") {
            PermuteObject::Collection1 collection1({"object1", "object2", "object3", "object4"});
            PermuteObject::Collection2 collection2({1, 2, 3, 4, 5, 6, 7, 8});
            const PermuteObject permute(collection1, collection2);

            vector<PermuteObjectElement> orderedCombinations;
            const size_t beginIndexObject1 = 1U;
            const size_t endIndexObject1 = collection1.size() - 1U;
            const size_t beginIndexObject2 = 1U;
            const size_t endIndexObject2 = collection2.size() - 1U;

            for(size_t i = beginIndexObject1; i < endIndexObject1; ++i) {
                for(size_t j = beginIndexObject2; j <endIndexObject2; ++j) {
                    PermuteObjectElement combination(collection1[i], collection2[j]);
                    orderedCombinations.push_back(combination);
                }
            }

            WHEN("We iterate over them partially") {
                THEN("We should get the same ordered combinations") {
                    size_t orderedCombinationsIndex = 0U;
                    for(auto it = PermutationIterator<const PermuteObjectElement, const PermuteObject::Collection1, const PermuteObject::Collection2>(permute.getCollection1().begin() + beginIndexObject1, permute.getCollection2().begin() + beginIndexObject2, permute.getCollection1().begin() + endIndexObject1, permute.getCollection2().begin() + endIndexObject2);
                            it != PermutationIterator<const PermuteObjectElement, const PermuteObject::Collection1, const PermuteObject::Collection2>(permute.getCollection1().begin() + endIndexObject1, permute.getCollection2().begin() + endIndexObject2, permute.getCollection1().begin() + endIndexObject1, permute.getCollection2().begin() + endIndexObject2);
                            ++it) {
                        REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                        REQUIRE(*it == orderedCombinations[orderedCombinationsIndex]);
                        ++orderedCombinationsIndex;
                    }
                    REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                }
            }
        }
    }
} } }
