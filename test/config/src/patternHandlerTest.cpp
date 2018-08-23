#include <iostream>
#include <vector>

#include <boost/optional/optional_io.hpp>
#include <gsl/string_span>

#include "config/patternsHandler.h"
#include "unittest/catch.h"

using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;

namespace {
const czstring<> DEFAULT_VALUES_KEY = "default-values";
const czstring<> SHORT_OPTION_KEY = "short-option";
const czstring<> LONG_OPTION_KEY = "long-option";

inline void checkContainsPatterns(const PatternsHandler& handler,
                                  const Patterns& expectedPatterns) {
    for(const auto& expectedPattern : expectedPatterns) {
        REQUIRE(handler.contains(expectedPattern.getKey()));
    }
}

inline void checkReturnsPatterns(const PatternsHandler& handler,
                                 const Patterns& expectedPatterns) {
    for(const auto& expectedPattern : expectedPatterns) {
        REQUIRE(handler.getPattern(expectedPattern.getKey()) ==
                expectedPattern);
    }
}

inline void checkNotContainsPatterns(const PatternsHandler& handler,
                                     const Patterns& /*expectedPatterns*/) {
    const Pattern notAddedPattern("not-added-pattern");
    REQUIRE_FALSE(handler.contains(notAddedPattern.getKey()));
}
} // namespace

namespace execHelper {
namespace config {
namespace test {
SCENARIO("Test the patterns the handler is holding",
         "[patterns][PatternsHandler]") {
    GIVEN("A patternshandler and some patterns to hold") {
        const Pattern pattern1("pattern1", {"pattern1A", "pattern1B"}, 'a',
                               string("patternA"));
        const Pattern pattern2("pattern2", {"pattern2A", "pattern2B"}, 'b',
                               string("patternB"));
        const Pattern pattern3("pattern3", {"pattern3A", "pattern3B"}, 'c',
                               string("patternC"));
        const Pattern pattern4("pattern4", {"pattern4A", "pattern4B"}, 'd',
                               string("patternD"));
        const Patterns patterns = {pattern1, pattern2, pattern3, pattern4};

        WHEN("We add the patterns separately") {
            PatternsHandler handler;

            for(const auto& pattern : patterns) {
                handler.addPattern(pattern);
            }

            THEN("It should contain the pattern") {
                checkContainsPatterns(handler, patterns);
            }

            THEN("It should return these patterns") {
                checkReturnsPatterns(handler, patterns);
            }

            THEN("It should not contain other patterns") {
                checkNotContainsPatterns(handler, patterns);
            }
        }

        WHEN("We add the patterns at construction time") {
            PatternsHandler handler(patterns);
            THEN("It should contain the pattern") {
                checkContainsPatterns(handler, patterns);
            }

            THEN("It should return these patterns") {
                checkReturnsPatterns(handler, patterns);
            }

            THEN("It should not contain other patterns") {
                checkNotContainsPatterns(handler, patterns);
            }
        }

        WHEN("We move construct the patternshandler using the patterns") {
            Patterns patternsToMove(patterns);
            PatternsHandler handler(move(patternsToMove));

            THEN("It should contain the pattern") {
                checkContainsPatterns(handler, patterns);
            }

            THEN("It should return these patterns") {
                checkReturnsPatterns(handler, patterns);
            }

            THEN("It should not contain other patterns") {
                checkNotContainsPatterns(handler, patterns);
            }
        }
    }
}

SCENARIO("Comparing two pattern handlers", "[patterns][PatternHandler]") {
    GIVEN("A pattern handler") {
        const Pattern pattern1("pattern1", {"pattern1A", "pattern1B"}, 'a',
                               string("patternA"));
        const Pattern pattern2("pattern2", {"pattern2A", "pattern2B"}, 'b',
                               string("patternB"));
        const Pattern pattern3("pattern3", {"pattern3A", "pattern3B"}, 'c',
                               string("patternC"));
        const Pattern pattern4("pattern4", {"pattern4A", "pattern4B"}, 'd',
                               string("patternD"));
        const vector<Pattern> patterns = {pattern1, pattern2, pattern3,
                                          pattern4};

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

SCENARIO("Test valid conversions to a pattern", "[patterns][PatternHandler]") {
    MAKE_COMBINATIONS("Of several valid combinations") {
        const PatternKey key("pattern1");
        PatternValues defaultValues({"default-value"});
        ShortOption shortOption;
        LongOption longOption;

        VariablesMap variables("test");

        // Add mandatory variable settings
        variables.add({DEFAULT_VALUES_KEY}, defaultValues);

        COMBINATIONS("Add a short option") {
            shortOption = 's';
            variables.add({SHORT_OPTION_KEY}, string(1, shortOption.get()));
        }

        COMBINATIONS("Add a long option") {
            longOption = "some-long-option";
            variables.add({LONG_OPTION_KEY}, longOption.get());
        }

        Pattern expectedPattern(key, defaultValues, shortOption, longOption);

        THEN_WHEN("We try to create a pattern") {
            auto actualPattern = PatternsHandler::toPattern(key, variables);

            THEN_CHECK("It should succeed") {
                REQUIRE(actualPattern != boost::none);
            }

            THEN_CHECK("We should find the expected pattern") {
                REQUIRE(expectedPattern == actualPattern);
            }
        }
    }
}

SCENARIO("Test invalid conversions to a pattern",
         "[patterns][PatternHandler]") {
    MAKE_COMBINATIONS("Of invalid combinations") {
        const PatternKey key("pattern1");

        VariablesMap variables("test");

        COMBINATIONS("Add empty default values") {
            variables.add(DEFAULT_VALUES_KEY);
        }

        COMBINATIONS("Add a short option") {
            variables.add({SHORT_OPTION_KEY}, "some-short-option");
        }

        COMBINATIONS("Add a long option") {
            variables.add({LONG_OPTION_KEY}, "some-long-option");
        }

        THEN_WHEN("We try to create a pattern") {
            auto returnCode = PatternsHandler::toPattern(key, variables);

            THEN_CHECK("It should fail") { REQUIRE(returnCode == boost::none); }
        }
    }
}
} // namespace test
} // namespace config
} // namespace execHelper
