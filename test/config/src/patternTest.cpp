#include <optional>
#include <sstream>
#include <string>

#include "config/pattern.h"
#include "unittest/catch.h"

using std::make_optional;
using std::string;
using std::stringstream;

using execHelper::config::Pattern;
using execHelper::config::PatternValues;

namespace execHelper::core::test {
SCENARIO("Test the pattern properties", "[patterns][Pattern]") {
    MAKE_COMBINATIONS("Some pattern properties") {
        const string key("key1");
        const PatternValues values({"value1a", "value1b"});
        auto shortOption = make_optional<char>();
        auto longOption = make_optional<string>();

        COMBINATIONS("Leave them as is") { ; }

        COMBINATIONS("Set the short option") { shortOption = '1'; }

        COMBINATIONS("Set the long option") { longOption = "long-option1"; }

        WHEN("We create a pattern") {
            Pattern pattern(key, values, shortOption, longOption);

            THEN("We should find these properties") {
                REQUIRE(pattern.getKey() == key);
                REQUIRE(pattern.getValues() == values);
                if(longOption) {
                    REQUIRE(pattern.getLongOption());
                    REQUIRE(pattern.getLongOption().value() == longOption);
                } else {
                    REQUIRE(pattern.getLongOption() == longOption);
                }
                if(shortOption) {
                    REQUIRE(pattern.getShortOption());
                    REQUIRE(pattern.getShortOption().value() == shortOption);
                } else {
                    REQUIRE(pattern.getShortOption() == shortOption);
                }
            }
        }
    }

    GIVEN("Empty pattern properties") {
        const string key;
        const PatternValues values({});
        const string longOption;

        WHEN("We create the pattern") {
            Pattern pattern(key, values, std::nullopt, longOption);

            THEN("We should find these properties") {
                REQUIRE(pattern.getKey() == key);
                REQUIRE(pattern.getValues() == values);
                REQUIRE(pattern.getLongOption() == longOption);
                REQUIRE(pattern.getShortOption() == std::nullopt);
            }
        }
    }
}

SCENARIO("Test the pattern facilities", "[patterns][Pattern]") {
    GIVEN("A test pattern") {
        const string key("test-key");
        const PatternValues values({"value1", "value2"});
        const char shortOption = 'o';
        const string longOption = "test-option";
        Pattern pattern(key, values, shortOption, longOption);

        WHEN("We copy the pattern") {
            Pattern
                other( // NOLINT(performance-unnecessary-copy-initialization)
                    pattern);

            THEN("We should get the same pattern") {
                REQUIRE(pattern == other);
                REQUIRE_FALSE(pattern != other);
            }
        }

        WHEN("We assign the pattern") {
            Pattern other("", {}, '\0', string("null"));
            other = pattern;

            THEN("We should get the same pattern") {
                REQUIRE(pattern == other);
                REQUIRE_FALSE(pattern != other);
            }
        }

        WHEN("We create similar patterns") {
            Pattern other1("other-key", values, shortOption, longOption);
            Pattern other2(key, {"value1"}, shortOption, longOption);
            Pattern other3(key, values, 'e', longOption);
            Pattern other4(key, values, shortOption, string("other-option"));

            THEN("They should not be equal") {
                REQUIRE(pattern != other1);
                REQUIRE(pattern != other2);
                REQUIRE(pattern != other3);
                REQUIRE(pattern != other4);

                REQUIRE_FALSE(pattern == other1);
                REQUIRE_FALSE(pattern == other2);
                REQUIRE_FALSE(pattern == other3);
                REQUIRE_FALSE(pattern == other4);
            }
        }
    }
}

SCENARIO("Test the pattern streaming operator", "[patterns][Pattern]") {
    GIVEN("A configured object with a short option to stream") {
        Pattern pattern("pattern-key", {"value1", "value2"}, '1',
                        string("long-option"));

        stringstream stream;

        WHEN("We apply the streaming operator") {
            stream << pattern;

            THEN("We should get the stream") {
                stringstream expectedStream;
                expectedStream << "{" << pattern.getKey() << ": ";
                expectedStream
                    << "short option: " << pattern.getShortOption().value()
                    << ", ";
                expectedStream
                    << "long option: " << pattern.getLongOption().value()
                    << ", ";
                expectedStream << "values: {";
                for(const auto& value : pattern.getValues()) {
                    expectedStream << value << ";";
                }
                expectedStream << "}";
                expectedStream << "}";
                REQUIRE(stream.str() == expectedStream.str());
            }
        }
    }
    GIVEN("A configured object with no short option to stream") {
        Pattern pattern("pattern-key", {"value1", "value2"}, std::nullopt,
                        string("long-option"));

        stringstream stream;

        WHEN("We apply the streaming operator") {
            stream << pattern;

            THEN("We should get the stream") {
                stringstream expectedStream;
                expectedStream << "{" << pattern.getKey() << ": ";
                expectedStream
                    << "long option: " << pattern.getLongOption().value()
                    << ", ";
                expectedStream << "values: {";
                for(const auto& value : pattern.getValues()) {
                    expectedStream << value << ";";
                }
                expectedStream << "}";
                expectedStream << "}";
                REQUIRE(stream.str() == expectedStream.str());
            }
        }
    }
}
} // namespace execHelper::core::test
