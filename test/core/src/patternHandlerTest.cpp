#include <vector>

#include <catch.hpp>

#include "core/patternsHandler.h"

using std::vector;

using execHelper::core::Pattern;
using execHelper::core::PatternsHandler;

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("Test the patterns the handler is holding", "[patterns][PatternsHandler]") {
            GIVEN("A patternshandler and some patterns to hold") {
                const Pattern pattern1("pattern1", {"pattern1A", "pattern1B"}, 'a', "patternA");
                const Pattern pattern2("pattern2", {"pattern2A", "pattern2B"}, 'b', "patternB");
                const Pattern pattern3("pattern3", {"pattern3A", "pattern3B"}, 'c', "patternC");
                const Pattern pattern4("pattern4", {"pattern4A", "pattern4B"}, 'd', "patternD");
                const vector<Pattern> patterns = {pattern1, pattern2, pattern3, pattern4};

                PatternsHandler handler;

                WHEN("We add the patterns") {
                    for(const auto& pattern : patterns) {
                        handler.addPattern(pattern);
                    }

                    THEN("It should contain the pattern") {
                        for(const auto& pattern : patterns) {
                            REQUIRE(handler.contains(pattern.getKey()));
                        }
                    }

                    THEN("It should return these patterns") {
                        for(const auto& pattern : patterns) {
                            REQUIRE(handler.getPattern(pattern.getKey()) == pattern);
                        }
                    }

                    THEN("It should not contain other patterns") {
                        const Pattern notAddedPattern("not-added-pattern", {}, 'n', "non-added");

                        REQUIRE_FALSE(handler.contains(notAddedPattern.getKey()));
                    }
                }
            }
        }

        SCENARIO("Comparing two pattern handlers", "[patterns][PatternHandler]") {
            GIVEN("A pattern handler") {
                const Pattern pattern1("pattern1", {"pattern1A", "pattern1B"}, 'a', "patternA");
                const Pattern pattern2("pattern2", {"pattern2A", "pattern2B"}, 'b', "patternB");
                const Pattern pattern3("pattern3", {"pattern3A", "pattern3B"}, 'c', "patternC");
                const Pattern pattern4("pattern4", {"pattern4A", "pattern4B"}, 'd', "patternD");
                const vector<Pattern> patterns = {pattern1, pattern2, pattern3, pattern4};

                PatternsHandler handler;

                for(const auto& pattern : patterns) {
                    handler.addPattern(pattern);
                }

                WHEN("We compare it with a similarly constructed handler") {
                    PatternsHandler other;

                    for(const auto& pattern : patterns) {
                        other.addPattern(pattern);
                    }

                    THEN("They should be equal") {
                        REQUIRE(handler == other);
                        REQUIRE_FALSE(handler != other);
                    }
                }

                WHEN("We compare it to a different handler") {
                    PatternsHandler other;

                    for(size_t i = 0U; i < patterns.size() - 2; ++i) {
                        other.addPattern(patterns[i]);
                    }

                    THEN("They should not be equal") {
                        REQUIRE_FALSE(handler == other);
                        REQUIRE(handler != other);
                    }
                }

                WHEN("We compare the handler with the default constructed handler") {
                    PatternsHandler other;

                    THEN("They should not be equal") {
                        REQUIRE_FALSE(handler == other);
                        REQUIRE(handler != other);
                    }
                }
            }
        }
    }
} }
