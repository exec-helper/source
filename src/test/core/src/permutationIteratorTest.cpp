#include <vector>
#include <deque>
#include <string>

#include <catch.hpp>

#include "core/permutationIterator.h"

using std::vector;
using std::deque;
using std::string;

using execHelper::core::PermutationIterator;
using execHelper::core::permutationBeginIterator;
using execHelper::core::permutationEndIterator;

namespace {
    class PermuteObject {
        public:
            typedef string Object1;
            typedef uint8_t Object2;
            typedef vector<Object1> Collection1;
            typedef deque<Object2> Collection2;

            typedef PermutationIterator<PermuteObject, Collection1, Collection2> iterator;
            typedef PermutationIterator<const PermuteObject, Collection1, Collection2> const_iterator;

            PermuteObject(const Collection1& collection1, const Collection2& collection2) noexcept :
                m_collection1(collection1),
                m_collection2(collection2)
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
                return permutationBeginIterator(*this, m_collection1, m_collection2);
            }

            const_iterator begin() const noexcept {
                return permutationBeginIterator(*this, m_collection1, m_collection2);
            }

            iterator end() noexcept {
                return permutationEndIterator(*this, m_collection1, m_collection2);
            }

            const_iterator end() const noexcept {
                return permutationEndIterator(*this, m_collection1, m_collection2);
            }

        private:
            Collection1 m_collection1;
            Collection2 m_collection2;

    };
}

namespace execHelper { namespace core { namespace test {
    SCENARIO("Test the permutation iterators when looping over the entire collection", "[permutationiterator]") {
        GIVEN("Some non-const collections to iterate over using permutations of its content and the ordered combinations") {
            PermuteObject permute({"object1", "object2"}, {1, 2});

            vector<PermuteObject> orderedCombinations;
            for(const auto& object1 : permute.getCollection1()) {
                for(const auto& object2 : permute.getCollection2()) {
                    PermuteObject combination({object1}, {object2});
                    orderedCombinations.push_back(combination);
                }
            }

            WHEN("We iterate over them entirely") {
                THEN("We should be able to do so using a foreach loop") {
                    size_t orderedCombinationsIndex = 0U;
                    for(auto combination : permute) {
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

        GIVEN("Some const collections to iterate over using permutations of its content and the ordered combinations") {
            const PermuteObject permute({"object1", "object2"}, {1, 2});

            vector<PermuteObject> orderedCombinations;
            for(const auto& object1 : permute.getCollection1()) {
                for(const auto& object2 : permute.getCollection2()) {
                    PermuteObject combination({object1}, {object2});
                    orderedCombinations.push_back(combination);
                }
            }

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

            vector<PermuteObject> orderedCombinations;
            const size_t beginIndexObject1 = 1U;
            const size_t endIndexObject1 = permute.getCollection1().size() - 1U;
            const size_t beginIndexObject2 = 1U;
            const size_t endIndexObject2 = permute.getCollection2().size() - 1U;

            for(size_t i = beginIndexObject1; i < endIndexObject1; ++i) {
                for(size_t j = beginIndexObject2; j < endIndexObject2; ++j) {
                    PermuteObject::Collection1 object1 = {permute.getCollection1()[i]};
                    PermuteObject::Collection2 object2 = {permute.getCollection2()[j]};

                    PermuteObject combination(object1, object2);
                    orderedCombinations.push_back(combination);
                }
            }

            WHEN("We iterate over them partially") {
                THEN("We should get the same ordered combinations") {
                    size_t orderedCombinationsIndex = 0U;
                    for(auto it = PermutationIterator<PermuteObject, PermuteObject::Collection1, PermuteObject::Collection2>(permute, permute.getCollection1().begin() + beginIndexObject1, permute.getCollection1().begin() + endIndexObject1, permute.getCollection2().begin() + beginIndexObject2, permute.getCollection2().begin() + endIndexObject2);
                            it != PermutationIterator<PermuteObject, PermuteObject::Collection1, PermuteObject::Collection2>(permute, permute.getCollection1().begin() + endIndexObject1, permute.getCollection1().begin() + endIndexObject1, permute.getCollection2().begin() + endIndexObject2, permute.getCollection2().begin() + endIndexObject2);
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
            const PermuteObject permute({"object1", "object2", "object3", "object4"}, {1, 2, 3, 4, 5, 6, 7, 8});

            vector<PermuteObject> orderedCombinations;
            const size_t beginIndexObject1 = 1U;
            const size_t endIndexObject1 = permute.getCollection1().size() - 1U;
            const size_t beginIndexObject2 = 1U;
            const size_t endIndexObject2 = permute.getCollection2().size() - 1U;

            for(size_t i = beginIndexObject1; i < endIndexObject1; ++i) {
                for(size_t j = beginIndexObject2; j < endIndexObject2; ++j) {
                    PermuteObject::Collection1 object1 = {permute.getCollection1()[i]};
                    PermuteObject::Collection2 object2 = {permute.getCollection2()[j]};

                    PermuteObject combination(object1, object2);
                    orderedCombinations.push_back(combination);
                }
            }

            WHEN("We iterate over them partially") {
                THEN("We should get the same ordered combinations") {
                    size_t orderedCombinationsIndex = 0U;
                    for(auto it = PermutationIterator<const PermuteObject, PermuteObject::Collection1, PermuteObject::Collection2>(permute, permute.getCollection1().begin() + beginIndexObject1, permute.getCollection1().begin() + endIndexObject1, permute.getCollection2().begin() + beginIndexObject2, permute.getCollection2().begin() + endIndexObject2);
                            it != PermutationIterator<const PermuteObject, PermuteObject::Collection1, PermuteObject::Collection2>(permute, permute.getCollection1().begin() + endIndexObject1, permute.getCollection1().begin() + endIndexObject1, permute.getCollection2().begin() + endIndexObject2, permute.getCollection2().begin() + endIndexObject2);
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
