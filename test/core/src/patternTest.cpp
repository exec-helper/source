#include <string>

#include "core/patterns.h"
#include "unittest/catch.h"

using std::string;

using execHelper::core::Pattern;

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("Test the pattern properties", "[patterns][Pattern]") {
            GIVEN("Some pattern properties") {
                const string key1("key1");
                const PatternValues values1({"value1a", "value1b"});
                const string longOption1("long-option1");
                const char shortOption1 = '1';

                WHEN("We create the pattern") {
                    Pattern pattern1(key1, values1, shortOption1, longOption1);

                    THEN("We should find these properties") {
                        REQUIRE(pattern1.getKey() == key1);
                        REQUIRE(pattern1.getDefaultValues() == values1);
                        REQUIRE(pattern1.getLongOption() == longOption1);
                        REQUIRE(pattern1.getShortOption() == shortOption1);
                    }
                }
            }

            GIVEN("Empty pattern properties") {
                const string key;
                const PatternValues values({});
                const string longOption;
                const char shortOption = '\0';

                WHEN("We create the pattern") {
                    Pattern pattern(key, values, shortOption, longOption);

                    THEN("We should find these properties") {
                        REQUIRE(pattern.getKey() == key);
                        REQUIRE(pattern.getDefaultValues() == values);
                        REQUIRE(pattern.getLongOption() == longOption);
                        REQUIRE(pattern.getShortOption() == shortOption);
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
                    Pattern other(pattern); // NOLINT(performance-unnecessary-copy-initialization)

                    THEN("We should get the same pattern") {
                        REQUIRE(pattern == other);
                        REQUIRE_FALSE(pattern != other);
                    }
                }

                WHEN("We assign the pattern") {
                    Pattern other("", {}, '\0', "");
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
                    Pattern other4(key, values, shortOption, "other-option");

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
    } // namespace test
} // namespace core
} // namespace execHelper
