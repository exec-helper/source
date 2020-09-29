#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "config/settingsNode.h"

#include "config/generators.h"
#include "unittest/catch.h"
#include "unittest/rapidcheck.h"
#include "utils/utils.h"

using std::move;
using std::nullopt;
using std::ostream;
using std::string;
using std::stringstream;
using std::to_string;

using Catch::Matchers::VectorContains;

using execHelper::config::SettingsNode;
using execHelper::config::SettingsValues;
using execHelper::test::propertyTest;

namespace {
void assertBoolSetting(const SettingsNode& settings, const string& key,
                       bool expected) {
    REQUIRE(settings.get<bool>(key) != std::nullopt);
    REQUIRE(settings.get<bool>(key).value() == expected);
    REQUIRE(settings.get<bool>(key, !expected) == expected);
}

const execHelper::config:: // NOLINT(fuchsia-statically-constructed-objects)
    SettingsValue DEFAULT_VALUE("blaat");
const execHelper::config:: // NOLINT(fuchsia-statically-constructed-objects)
    SettingsValues DEFAULT_VALUES({DEFAULT_VALUE});
} // namespace

namespace execHelper::config::test {
SCENARIO("Basic addition and getting of values", "[config][settingsNode]") {
    GIVEN("A basic setup") {
        const SettingsKey rootKey("root-key");
        const SettingsValue testValue1("test-value1");
        const SettingsKey testKey2("test-key2");
        const SettingsValue testValue2("test-value2");
        const SettingsValues testValue3({"test-value3a", "test-value3b"});
        const SettingsKeys rootKeys({testValue1, testKey2});
        const std::vector<string> testValue4(
            {"test-value4a", "test-value4b",
             "test-value4c"}); // Note: due to the lifetime of an
                               // initializer_list in c++ 11, we can not use an
                               // initializer_list object here.
        const SettingsKey testKey5("test-key5");
        const SettingsValues testValue5({"test-value5a", "test-value5b"});
        const SettingsKey testKey6("test-key6");
        const std::vector<string> testValue6(
            {"test-value6a", "test-value6b"}); // Note: due to the lifetime of
                                               // an initializer_list in c++ 11,
                                               // we can not use an
                                               // initializer_list object here.

        SettingsNode settings(rootKey);

        WHEN("We get the key") {
            const auto& resultRootKey = settings.key();

            THEN("It should match") { REQUIRE(resultRootKey == rootKey); }
        }

        WHEN("We add values") {
            REQUIRE(settings.add(testValue1));
            REQUIRE(settings.add({testKey2}, testValue2));
            REQUIRE(settings.add(testValue3));
            REQUIRE(
                settings.add({"test-value4a", "test-value4b", "test-value4c"}));
            REQUIRE(settings.add({testKey5}, testValue5));
            REQUIRE(settings.add({testKey6}, {"test-value6a", "test-value6b"}));

            THEN("The settings should contain them") {
                REQUIRE(settings.contains(testValue1));
                REQUIRE(settings.contains({testKey2}));
                REQUIRE(settings[testKey2].contains(testValue2));
                for(const auto& key : testValue3) {
                    REQUIRE(settings.contains(key));
                }
                for(const auto& key : testValue4) {
                    REQUIRE(settings.contains(key));
                }
                REQUIRE(settings.contains(testKey5));
                for(const auto& value : testValue5) {
                    REQUIRE(settings[testKey5].contains(value));
                }
                REQUIRE(settings.contains(testKey6));
                for(const auto& value : testValue6) {
                    REQUIRE(settings[testKey6].contains(value));
                }
            }

            THEN("The keys should exist") {
                REQUIRE(settings[testValue1].key() == testValue1);
                REQUIRE(settings[testKey2].key() == testKey2);
                REQUIRE(settings[testKey2][testValue2].key() == testValue2);
                for(const auto& key : testValue3) {
                    REQUIRE(settings[key].key() == key);
                }

                for(const auto& key : testValue4) {
                    REQUIRE(settings[key].key() == key);
                }

                REQUIRE(settings[testKey5].key() == testKey5);
                for(const auto& key : testValue5) {
                    REQUIRE(settings[testKey5][key].key() == key);
                }

                REQUIRE(settings[testKey6].key() == testKey6);
                for(const auto& key : testValue6) {
                    REQUIRE(settings[testKey6][key].key() == key);
                }
            }

            THEN("We should be able to get the optional values") {
                REQUIRE(settings.get<SettingsValues>(SettingsKeys()) !=
                        std::nullopt);
                REQUIRE_THAT(
                    settings.get<SettingsValues>(SettingsKeys()).value(),
                    VectorContains(testValue1));

                REQUIRE(settings.get<SettingsValues>({testValue1}) ==
                        std::nullopt);

                REQUIRE(settings.get<SettingsValues>({testKey2}) !=
                        std::nullopt);
                REQUIRE(settings.get<SettingsValues>({testKey2}).value() ==
                        SettingsValues({testValue2}));

                for(const auto& key : testValue3) {
                    REQUIRE(settings.get<SettingsValues>({key}) ==
                            std::nullopt);
                    REQUIRE_THAT(
                        settings.get<SettingsValues>(SettingsKeys()).value(),
                        VectorContains(key));
                }

                for(const auto& key : testValue4) {
                    REQUIRE(settings.get<SettingsValues>({key}) ==
                            std::nullopt);
                    REQUIRE_THAT(
                        settings.get<SettingsValues>(SettingsKeys()).value(),
                        VectorContains(key));
                }

                REQUIRE(settings.get<SettingsValues>({testKey5}) !=
                        std::nullopt);
                REQUIRE(settings.get<SettingsValues>({testKey5}).value() ==
                        testValue5);

                REQUIRE(settings[testKey5].get<SettingsValues>(
                            SettingsKeys()) != std::nullopt);
                REQUIRE(settings[testKey5]
                            .get<SettingsValues>(SettingsKeys())
                            .value() == testValue5);

                for(const auto& key : testValue5) {
                    REQUIRE(settings.get<SettingsValues>({testKey5, key}) ==
                            std::nullopt);

                    REQUIRE(settings[testKey5].get<SettingsValues>({key}) ==
                            std::nullopt);
                }
                REQUIRE(settings.get<SettingsValues>(
                            {testKey5, "non-existing-key"}) == std::nullopt);

                REQUIRE(settings.get<SettingsValues>({testKey6}) !=
                        std::nullopt);
                REQUIRE(settings.get<SettingsValues>({testKey6}).value() ==
                        SettingsValues(testValue6));

                REQUIRE(settings[testKey6].get<SettingsValues>(
                            SettingsKeys()) != std::nullopt);
                REQUIRE(settings[testKey6]
                            .get<SettingsValues>(SettingsKeys())
                            .value() == testValue6);

                for(const auto& key : testValue6) {
                    REQUIRE(settings.get<SettingsValues>({testKey6, key}) ==
                            std::nullopt);

                    REQUIRE(settings[testKey6].get<SettingsValues>({key}) ==
                            std::nullopt);
                }
            }

            THEN("We should be able to get the non-default values") {
                const SettingsValues DEFAULT_VALUE({"blaat"});

                REQUIRE_THAT(
                    settings.get<SettingsValues>(SettingsKeys(), DEFAULT_VALUE),
                    VectorContains(testValue1));

                REQUIRE(settings.get<SettingsValues>(
                            testValue1, DEFAULT_VALUE) == DEFAULT_VALUE);

                REQUIRE(
                    settings.get<SettingsValues>({testKey2}, DEFAULT_VALUE) ==
                    SettingsValues({testValue2}));

                for(const auto& key : testValue3) {
                    REQUIRE(settings.get<SettingsValues>(key, DEFAULT_VALUE) ==
                            DEFAULT_VALUE);
                    REQUIRE_THAT(settings.get<SettingsValues>(SettingsKeys(),
                                                              DEFAULT_VALUE),
                                 VectorContains(key));
                }

                for(const auto& key : testValue4) {
                    REQUIRE(settings.get<SettingsValues>(key, DEFAULT_VALUE) ==
                            DEFAULT_VALUE);
                    REQUIRE_THAT(settings.get<SettingsValues>(SettingsKeys(),
                                                              DEFAULT_VALUE),
                                 VectorContains(key));
                }

                REQUIRE(settings.get<SettingsValues>(
                            {testKey5}, DEFAULT_VALUE) == testValue5);

                REQUIRE(settings[testKey5].get<SettingsValues>(
                            SettingsKeys(), DEFAULT_VALUE) == testValue5);
                for(const auto& key : testValue5) {
                    REQUIRE(settings.get<SettingsValues>({testKey5, key},
                                                         DEFAULT_VALUE) ==
                            DEFAULT_VALUE);
                    REQUIRE(settings[testKey5].get<SettingsValues>(
                                {key}, DEFAULT_VALUE) == DEFAULT_VALUE);
                }
                REQUIRE(settings.get<SettingsValues>(
                            {testKey5, "non-existing-key"}, DEFAULT_VALUE) ==
                        DEFAULT_VALUE);

                REQUIRE(settings.get<SettingsValues>(
                            {testKey6}, DEFAULT_VALUE) == testValue6);

                REQUIRE(settings[testKey6].get<SettingsValues>(
                            SettingsKeys(), DEFAULT_VALUE) == testValue6);
                REQUIRE(
                    settings.get<SettingsValues>({testKey6}, DEFAULT_VALUE) ==
                    SettingsValues(testValue6));
                for(const auto& key : testValue6) {
                    REQUIRE(settings.get<SettingsValues>({testKey6, key},
                                                         DEFAULT_VALUE) ==
                            DEFAULT_VALUE);
                }
            }
        }
    }
}

SCENARIO("Test various ways to set a boolean value", "[config][settingsNode]") {
    GIVEN("A settings node") {
        const SettingsKey rootKey("root-key");
        SettingsNode settings(rootKey);

        WHEN("We set the boolean value as an integer") {
            const SettingsKey falseInt("false-int");
            const SettingsKey trueInt("true-int");
            REQUIRE(settings.add(falseInt, "0"));
            REQUIRE(settings.add(trueInt, "1"));

            THEN("The false int must map to false") {
                assertBoolSetting(settings, falseInt, false);
            }

            THEN("The true int must map to false") {
                assertBoolSetting(settings, trueInt, true);
            }
        }

        WHEN("We set the boolean values as true or false") {
            const SettingsKey falseKey("false-key");
            const SettingsKey trueKey("true-key");
            REQUIRE(settings.add(falseKey, "false"));
            REQUIRE(settings.add(trueKey, "true"));

            THEN("The false key must map to false") {
                assertBoolSetting(settings, falseKey, false);
            }

            THEN("The true key must map to true") {
                assertBoolSetting(settings, trueKey, true);
            }
        }

        WHEN("We set the boolean values as yes or no") {
            const SettingsKey falseKey("false-key");
            const SettingsKey trueKey("true-key");
            REQUIRE(settings.add(falseKey, "no"));
            REQUIRE(settings.add(trueKey, "yes"));

            THEN("The false key must map to false") {
                assertBoolSetting(settings, falseKey, false);
            }

            THEN("The true key must map to true") {
                assertBoolSetting(settings, trueKey, true);
            }
        }
    }
}

SCENARIO("Addition of multiple key values", "[config][settingsNode]") {
    GIVEN("A basic setup") {
        const SettingsKey rootKey("root-key");
        const SettingsKeys key1({"key1a", "key1b", "key1c", "key1d"});
        const SettingsValues value1({"value1a", "value1b", "value1c"});
        SettingsNode settings(rootKey);

        WHEN("We add the values") {
            REQUIRE(settings.add(key1, value1));

            THEN("It should contain it") {
                SettingsKeys searchKeys;
                for(const auto& key : key1) {
                    searchKeys.emplace_back(key);
                    REQUIRE(settings.contains(searchKeys));
                }
                REQUIRE(settings.contains(key1));
            }
            THEN("We should get the values using the [] operator") {
                const SettingsNode* stageSettings = &settings;
                for(const auto& key : key1) {
                    stageSettings = &stageSettings->operator[](key);
                }
                REQUIRE(stageSettings->get<SettingsValues>(
                            SettingsKeys(), SettingsValues()) == value1);
            }
            THEN("We should get the values") {
                REQUIRE(settings.get<SettingsValues>(key1) != std::nullopt);
                REQUIRE(settings.get<SettingsValues>(key1).value() == value1);
            }
            THEN("We should get the values, not the default ones") {
                const SettingsValues DEFAULT_VALUE({"blaat"});

                REQUIRE(settings.get<SettingsValues>(key1, DEFAULT_VALUE) ==
                        value1);
            }
        }
    }
    GIVEN("A settingsnode with a deep hierarchy") {
        SettingsKeys veryLongKeys1;
        const SettingsValues value1({"value1a", "value1b"});
        SettingsKeys veryLongKeys2;
        const SettingsValues value2({"value2a", "value2b"});
        SettingsNode settings("root-key");

        const size_t TEST_SIZE_DEPTH = 8192U;

        for(size_t i = 0; i < TEST_SIZE_DEPTH; ++i) {
            veryLongKeys1.push_back("key1-" + to_string(i));
            veryLongKeys2.push_back("key2-" + to_string(i));
        }

        WHEN("We add the keys") {
            REQUIRE(settings.add(veryLongKeys1, value1));
            REQUIRE(settings.add(veryLongKeys2, value2));

            THEN("It should contain these") {
                REQUIRE(settings.get<SettingsValues>(veryLongKeys1) !=
                        std::nullopt);
                REQUIRE(settings.get<SettingsValues>(veryLongKeys1).value() ==
                        value1);

                REQUIRE(settings.get<SettingsValues>(veryLongKeys2) !=
                        std::nullopt);
                REQUIRE(settings.get<SettingsValues>(veryLongKeys2).value() ==
                        value2);
            }
        }
    }
    GIVEN("A settingsnode with a broad hierarchy") {
        const SettingsKeys key1({"key1a", "key1b"});
        const SettingsKeys key2({"key2a", "key2b"});
        SettingsNode settings("root-key");

        const size_t TEST_SIZE_BREADTH = 8192U;

        WHEN("We add the keys") {
            SettingsValues value1;
            SettingsValues value2;

            for(size_t i = 0; i < TEST_SIZE_BREADTH; ++i) {
                const SettingsValue newValue1 = "value1-" + to_string(i);
                const SettingsValue newValue2 = "value2-" + to_string(i);

                REQUIRE(settings.add(key1, newValue1));
                REQUIRE(settings.add(key2, newValue2));

                value1.push_back(newValue1);
                value2.push_back(newValue2);
            }

            THEN("It should contain these") {
                REQUIRE(settings.get<SettingsValues>(key1) != std::nullopt);
                REQUIRE(settings.get<SettingsValues>(key1).value() == value1);

                REQUIRE(settings.get<SettingsValues>(key2) != std::nullopt);
                REQUIRE(settings.get<SettingsValues>(key2).value() == value2);
            }
        }
        WHEN("We add the keys as a whole") {
            SettingsValues value1;
            SettingsValues value2;

            for(size_t i = 0; i < TEST_SIZE_BREADTH; ++i) {
                const SettingsValue newValue1 = "value1-" + to_string(i);
                const SettingsValue newValue2 = "value2-" + to_string(i);

                value1.push_back(newValue1);
                value2.push_back(newValue2);
            }

            REQUIRE(settings.add(key1, value1));
            REQUIRE(settings.add(key2, value2));

            THEN("It should contain these") {
                REQUIRE(settings.get<SettingsValues>(key1) != std::nullopt);
                REQUIRE(settings.get<SettingsValues>(key1).value() == value1);

                REQUIRE(settings.get<SettingsValues>(key2) != std::nullopt);
                REQUIRE(settings.get<SettingsValues>(key2).value() == value2);
            }
        }
    }
}

SCENARIO("Testing the (in)equality operator", "[config][settingsNode]") {
    GIVEN("A setup settings node") {
        const SettingsKey rootKey("root-key");
        const SettingsKeys key1({"key1a", "key1b", "key1c", "key1d"});
        const SettingsValues value1({"value1a", "value1b", "value1c"});
        const SettingsKeys key2(
            {"key2a", "key2b", "key2c", "key2d", "key2e", "key2f"});
        const SettingsValues value2(
            {"value2a", "value2b", "value2c", "value2d"});

        SettingsNode settings(rootKey);
        REQUIRE(settings.add(key1, value1));
        REQUIRE(settings.add(key2, value2));

        WHEN("We create a similar settings node") {
            SettingsNode similarSettings(rootKey);
            REQUIRE(similarSettings.add(key1, value1));
            REQUIRE(similarSettings.add(key2, value2));

            THEN("They should be equal") {
                REQUIRE(settings == similarSettings);
                REQUIRE_FALSE(settings != similarSettings);
            }
        }
        WHEN("We create a similar settings node with a different root key") {
            SettingsNode similarSettings("other-root-key");
            REQUIRE(similarSettings.add(key1, value1));
            REQUIRE(similarSettings.add(key2, value2));

            THEN("They should not be equal") {
                REQUIRE_FALSE(settings == similarSettings);
                REQUIRE(settings != similarSettings);
            }
        }
        WHEN("We create a similar settings node with a different value") {
            SettingsNode similarSettings("root-key");
            REQUIRE(similarSettings.add(key1, value2));

            THEN("They should not be equal") {
                REQUIRE_FALSE(settings == similarSettings);
                REQUIRE(settings != similarSettings);
            }
        }
        WHEN("We create an almost similar settings node") {
            SettingsNode similarSettings(rootKey);
            REQUIRE(similarSettings.add(key1, value1));

            THEN("They should not be equal") {
                REQUIRE_FALSE(settings == similarSettings);
                REQUIRE(settings != similarSettings);
            }
        }
        WHEN("We create an almost similar settings node") {
            SettingsNode similarSettings(rootKey);
            REQUIRE(similarSettings.add(key2, value2));

            THEN("They should not be equal") {
                REQUIRE_FALSE(settings == similarSettings);
                REQUIRE(settings != similarSettings);
            }
        }
        WHEN("We compare with an empty settings node") {
            SettingsNode similarSettings(rootKey);

            THEN("They should not be equal") {
                REQUIRE_FALSE(settings == similarSettings);
                REQUIRE(settings != similarSettings);
            }
        }
    }

    GIVEN("Two empty settings nodes") {
        const SettingsKey rootKey("root-key");
        SettingsNode similarSettings1(rootKey);
        SettingsNode similarSettings2(rootKey);

        WHEN("We compare two empty settings nodes") {
            THEN("They should not be equal") {
                REQUIRE(similarSettings1 == similarSettings2);
                REQUIRE_FALSE(similarSettings1 != similarSettings2);
            }
        }
    }
}

SCENARIO("Test the copy constructor", "[settingsNode]") {
    propertyTest("A settings node to copy", [](const SettingsNode& expected) {
        THEN_WHEN("We copy the settings") {
            SettingsNode
                actual( // NOLINT(performance-unnecessary-copy-initialization)
                    expected);

            THEN_CHECK("The objects must be equal") {
                REQUIRE(actual == expected);
            }

            THEN_CHECK(
                "The objects must exist on a different place in memory") {
                REQUIRE(&expected != &actual);
                REQUIRE(&(expected.key()) != &(actual.key()));
            }
        }
    });
}

SCENARIO("Test the copy assignment operator", "[settingsNode]") {
    propertyTest("A settings node to copy", [](const SettingsNode& expected,
                                               std::string&& key) {
        THEN_WHEN("We copy the settings") {
            SettingsNode actual(
                key); // NOLINT(performance-unnecessary-copy-initialization)
            actual =
                expected; // NOLINT(performance-unnecessary-copy-initialization)

            AND_THEN("The objects must be equal") {
                REQUIRE(actual == expected);
            }

            AND_THEN("The objects must exist on a different place in memory") {
                REQUIRE(&expected != &actual);
                REQUIRE(&(expected.key()) != &(actual.key()));
            }
        }
    });
}

SCENARIO("Test the move constructor", "[settingsNode]") {
    propertyTest("A settings node to move", [](const SettingsNode& expected) {
        THEN_WHEN("We copy the settings") {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpessimizing-move"
            SettingsNode
                actual(   // NOLINT(performance-unnecessary-copy-initialization)
                    move( // NOLINT(clang-diagnostic-pessimizing-move)
                        SettingsNode(expected)));
#pragma GCC diagnostic pop

            AND_THEN("The objects must be equal") {
                REQUIRE(actual == expected);
            }

            AND_THEN("The objects must exist on a different place in memory") {
                REQUIRE(&expected != &actual);
                REQUIRE(&(expected.key()) != &(actual.key()));
            }
        }
    });
}

SCENARIO("Test the move assignment operator", "[settingsNode]") {
    propertyTest("A settings node to move", [](const SettingsNode& expected,
                                               std::string&& key) {
        THEN_WHEN("We copy the settings") {
            SettingsNode
                intermediateCopy( // NOLINT(performance-unnecessary-copy-initialization)
                    expected);
            SettingsNode actual(key);
            actual = move(
                intermediateCopy); // NOLINT(performance-unnecessary-copy-initialization)

            AND_THEN("The objects must be equal") {
                REQUIRE(actual == expected);
            }

            AND_THEN("The objects must exist on a different place in memory") {
                REQUIRE(&expected != &actual);
                REQUIRE(&(expected.key()) != &(actual.key()));
            }
        }
    });
}

SCENARIO("Testing the removal of values", "[config][settingsNode]") {
    GIVEN("A settings node setup with some values") {
        const SettingsKey rootKey("root-key");
        const SettingsKeys key1({"key1"});
        const SettingsKeys key2({"key2"});
        const SettingsKeys key3({"key3"});
        const SettingsValues value1({"value1a", "value1b"});
        const SettingsValues value2({"value2a", "value2b"});

        SettingsNode settings(rootKey);
        REQUIRE(settings.add(key1, value1));
        REQUIRE(settings.add(key2, value2));
        REQUIRE(settings.add(key3));

        WHEN("We clear a key with values") {
            bool returnCode = settings.clear(key1);

            THEN("It should succeed") { REQUIRE(returnCode); }

            THEN("They should be cleared") {
                REQUIRE(settings.get<SettingsValues>(key1) == std::nullopt);

                const SettingsValues DEFAULT_VALUE({"blaat"});
                REQUIRE(settings.get<SettingsValues>(key1, DEFAULT_VALUE) ==
                        DEFAULT_VALUE);
            }

            THEN("The other key should be untouched") {
                const SettingsValues DEFAULT_VALUE({"blaat"});
                REQUIRE(settings.get<SettingsValues>(key2, DEFAULT_VALUE) ==
                        value2);
                REQUIRE(settings.get<SettingsValues>(key3, DEFAULT_VALUE) ==
                        DEFAULT_VALUE);
            }
        }

        WHEN("We clear a key with no values") {
            bool returnCode = settings.clear(key3);

            THEN("It should succeed") { REQUIRE(returnCode); }

            THEN("They should be cleared") {
                REQUIRE(settings.get<SettingsValues>(key3) == std::nullopt);

                const SettingsValues DEFAULT_VALUE({"blaat"});
                REQUIRE(settings.get<SettingsValues>(key3, DEFAULT_VALUE) ==
                        DEFAULT_VALUE);
            }

            THEN("The other key should be untouched") {
                const SettingsValues DEFAULT_VALUE({"blaat"});
                REQUIRE(settings.get<SettingsValues>(key1, DEFAULT_VALUE) ==
                        value1);
                REQUIRE(settings.get<SettingsValues>(key2, DEFAULT_VALUE) ==
                        value2);
            }
        }

        WHEN("We clear with an empty key") {
            bool returnCode = settings.clear(SettingsKeys());

            THEN("It should fail") { REQUIRE_FALSE(returnCode); }
        }

        WHEN("We clear a non-existing key") {
            bool returnCode1 =
                settings.clear(SettingsKeys({"non-existing-key"}));
            bool returnCode2 = settings.clear(
                SettingsKeys({key1.front(), "non-existing-key"}));

            THEN("It should succeed") {
                REQUIRE(returnCode1);
                REQUIRE(returnCode2);
            }
            THEN("The other keys should be untouched") {
                const SettingsValues DEFAULT_VALUE({"blaat"});
                REQUIRE(settings.get<SettingsValues>(key1, DEFAULT_VALUE) ==
                        value1);
                REQUIRE(settings.get<SettingsValues>(key2, DEFAULT_VALUE) ==
                        value2);
                REQUIRE(settings.get<SettingsValues>(key3, DEFAULT_VALUE) ==
                        DEFAULT_VALUE);
            }
        }
    }
}

SCENARIO("Test the settings node streaming operator",
         "[config][settingsNode]") {
    GIVEN("A configured settings object to assign and a stream") {
        const SettingsKey rootKey("root-key");
        const SettingsKey rootKey2("root-key2");
        const SettingsKeys key1({"key1"});
        const SettingsValues value1({"value1a", "value1b"});
        const SettingsKeys key2({"key2"});
        const SettingsValues value2({"value2a", "value2b"});
        const SettingsKeys key3({"key3a", "key3b"});
        const SettingsValues value3({"value3"});

        SettingsNode settings(rootKey);
        REQUIRE(settings.add(key1, value1));
        REQUIRE(settings.add(key2, value2));
        REQUIRE(settings.add(key3, value3));
        const SettingsNode constSettings =
            settings; // NOLINT(performance-unnecessary-copy-initialization)

        stringstream stream;

        WHEN("We apply the streaming operator to the stream") {
            stream << settings;

            THEN("We should get the stream") {
                stringstream correctStream;
                correctStream << "- " << rootKey << ":" << std::endl;
                correctStream << "  - " << key1.front() << ":" << std::endl;
                for(const auto& value : value1) {
                    correctStream << "    - " << value << std::endl;
                }
                correctStream << "  - " << key2.front() << ":" << std::endl;
                for(const auto& value : value2) {
                    correctStream << "    - " << value << std::endl;
                }
                correctStream << "  - " << key3.front() << ":" << std::endl;
                correctStream << "    - " << key3.back() << ":" << std::endl;
                for(const auto& value : value3) {
                    correctStream << "      - " << value << std::endl;
                }
                REQUIRE(stream.str() == correctStream.str());
            }
        }
    }
}

SCENARIO("Test adding values", "[config][settingsNode]") {
    propertyTest("Add one value with a simple key",
                 [](const SettingsKey& key, const SettingsValue& value) {
                     SettingsNode settings("Addition test");
                     REQUIRE(!settings.contains(key));
                     REQUIRE(settings.get<SettingsValue>(key) == std::nullopt);
                     REQUIRE(settings.get<SettingsValue>(key, DEFAULT_VALUE) ==
                             DEFAULT_VALUE);

                     REQUIRE(settings.add(key, value));
                     REQUIRE(settings.contains(key));
                     REQUIRE(settings.get<SettingsValue>(key) == value);
                     REQUIRE(settings.get<SettingsValue>(key, DEFAULT_VALUE) ==
                             value);
                 });

    propertyTest("Add one value with a key", [](const SettingsKeys& key,
                                                const SettingsValue& value) {
        SettingsNode settings("Addition test");
        REQUIRE(
            settings.contains(key) ==
            key.empty()); // An empty key will return true, as this points to the root settings node itself
        REQUIRE(settings.get<SettingsValue>(key) == std::nullopt);
        REQUIRE(settings.get<SettingsValue>(key, DEFAULT_VALUE) ==
                DEFAULT_VALUE);

        REQUIRE(settings.add(key, value));
        REQUIRE(settings.contains(key));
        REQUIRE(settings.get<SettingsValue>(key) == value);
        REQUIRE(settings.get<SettingsValue>(key, DEFAULT_VALUE) == value);
    });

    propertyTest("Add multiple values with a simple key",
                 [](const SettingsKey& key, const SettingsValues& values) {
                     SettingsNode settings("Addition test");
                     REQUIRE(!settings.contains(key));
                     REQUIRE(settings.get<SettingsValues>(key) == std::nullopt);
                     REQUIRE(settings.get<SettingsValues>(
                                 key, DEFAULT_VALUES) == DEFAULT_VALUES);

                     REQUIRE(settings.add(key, values));
                     REQUIRE(settings.contains(key));
                     REQUIRE(settings.get<SettingsValues>(key) == values);
                     REQUIRE(settings.get<SettingsValues>(
                                 key, DEFAULT_VALUES) == values);
                 });

    propertyTest("Add multiple elements with a key", [](const SettingsKeys& key,
                                                        const SettingsValues&
                                                            values) {
        SettingsNode settings("Addition test");
        REQUIRE(
            settings.contains(key) ==
            key.empty()); // An empty key will return true, as this points to the root settings node itself
        REQUIRE(settings.get<SettingsValues>(key) == std::nullopt);
        REQUIRE(settings.get<SettingsValues>(key, DEFAULT_VALUES) ==
                DEFAULT_VALUES);

        REQUIRE(settings.add(key, values));
        REQUIRE(settings.contains(key));
        REQUIRE(settings.get<SettingsValues>(key) == values);
        REQUIRE(settings.get<SettingsValues>(key, DEFAULT_VALUES) == values);
    });
}
} // namespace execHelper::config::test
