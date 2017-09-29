#include <iostream>
#include <string>
#include <vector>

// Hack required for streaming boost::optional<vector<T>> types
namespace std {     // NOLINT(cert-dcl58-cpp)
    template<typename T>
    std::ostream& operator<<(std::ostream& os, const std::vector<T>& stream) {
        for(const auto& element : stream) {
            os << element << ", ";
        }
        return os;
    }
} // namespace std

#include <boost/optional/optional_io.hpp>
#include <catch.hpp>

#include "config/settingsNode.h"

using std::string;
using std::to_string;
using std::ostream;

using execHelper::config::SettingsNode;

namespace execHelper { namespace config { namespace test {
    SCENARIO("Basic addition and getting of values", "[config][settingsNode]") {
        GIVEN("A basic setup") {
            const SettingsNode::SettingsKey rootKey("root-key");
            const SettingsNode::SettingsValue testValue1("test-value1");
            const SettingsNode::SettingsKey testKey2("test-key2");
            const SettingsNode::SettingsValue testValue2("test-value2");
            const SettingsNode::SettingsValues testValue3({"test-value3a", "test-value3b"});
            const SettingsNode::SettingsKeys rootKeys({testValue1, testKey2});
            const std::vector<string> testValue4({"test-value4a", "test-value4b", "test-value4c"}); // Note: due to the lifetime of an initializer_list in c++ 11, we can not use an initializer_list object here.
            const SettingsNode::SettingsKey testKey5("test-key5");
            const SettingsNode::SettingsValues testValue5({"test-value5a", "test-value5b"});
            const SettingsNode::SettingsKey testKey6("test-key6");
            const std::vector<string> testValue6({"test-value6a", "test-value6b"}); // Note: due to the lifetime of an initializer_list in c++ 11, we can not use an initializer_list object here.

            SettingsNode settings(rootKey);

            WHEN("We get the key") {
                const SettingsNode::SettingsKey resultRootKey = settings.key();

                THEN("It should match") {
                    REQUIRE(resultRootKey == rootKey);
                }
            }

            WHEN("We add values") {
                REQUIRE(settings.add(testValue1));
                REQUIRE(settings.add({testKey2}, testValue2));
                REQUIRE(settings.add(testValue3));
                REQUIRE(settings.add({"test-value4a", "test-value4b", "test-value4c"}));
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

                THEN("We should get the values") {
                    SettingsNode::SettingsValues firstLevelValues({testValue1, testKey2});
                    for(const auto& value : testValue3) {
                        firstLevelValues.push_back(value);
                    }
                    for(const auto& value : testValue4) {
                        firstLevelValues.push_back(value);
                    }
                    firstLevelValues.push_back(testKey5);
                    firstLevelValues.push_back(testKey6);

                    REQUIRE(settings.values() == firstLevelValues);
                    REQUIRE(settings[testValue1].values().empty());
                    REQUIRE(settings[testKey2].values() == SettingsNode::SettingsValues({testValue2}));
                    for(const auto& key : testValue3) {
                        REQUIRE(settings[key].values().empty());
                    }

                    for(const auto& key : testValue4) {
                        REQUIRE(settings[key].values().empty());
                    }

                    REQUIRE(settings[testKey5].values() == testValue5);
                    for(const auto& key : testValue5) {
                        REQUIRE(settings[testKey5][key].values().empty());
                    }

                    REQUIRE(settings[testKey6].values() == SettingsNode::SettingsValues(testValue6));
                    for(const auto& key : testValue6) {
                        REQUIRE(settings[testKey6][key].values().empty());
                    }
                }

                THEN("We should be able to get the optional values") {
                    REQUIRE(settings.get({testValue1}) == boost::none);
                    REQUIRE(settings.get({testKey2}) != boost::none);
                    REQUIRE(settings.get({testKey2}).get() == SettingsNode::SettingsValues({testValue2}));

                    for(const auto& key : testValue3) {
                        REQUIRE(settings.get({key}) == boost::none);
                    }

                    for(const auto& key : testValue4) {
                        REQUIRE(settings.get({key}) == boost::none);
                    }

                    REQUIRE(settings.get({testKey5}) != boost::none);
                    REQUIRE(settings.get({testKey5}).get() == testValue5);

                    for(const auto& key : testValue5) {
                        REQUIRE(settings.get({testKey5, key}) == boost::none);
                        REQUIRE(settings[testKey5].get({key}) == boost::none);
                    }

                    REQUIRE(settings.get({testKey6}) != boost::none);
                    REQUIRE(settings.get({testKey6}).get() == SettingsNode::SettingsValues(testValue6));

                    for(const auto& key : testValue6) {
                        REQUIRE(settings.get({testKey6, key}) == boost::none);
                        REQUIRE(settings[testKey6].get({key}) == boost::none);
                    }
                }
                
                THEN("We should be able to get the non-default values") {
                    const SettingsNode::SettingsValues DEFAULT_VALUE({"blaat"});

                    REQUIRE(settings.get({testValue1}, DEFAULT_VALUE) == DEFAULT_VALUE);
                    REQUIRE(settings.get({testKey2}, DEFAULT_VALUE) == SettingsNode::SettingsValues({testValue2}));

                    for(const auto& key : testValue3) {
                        REQUIRE(settings.get({key}, DEFAULT_VALUE) == DEFAULT_VALUE);
                    }

                    for(const auto& key : testValue4) {
                        REQUIRE(settings.get({key}, DEFAULT_VALUE) == DEFAULT_VALUE);
                    }

                    REQUIRE(settings.get({testKey5}, DEFAULT_VALUE) == testValue5);
                    for(const auto& key : testValue5) {
                        REQUIRE(settings.get({testKey5, key}, DEFAULT_VALUE) == DEFAULT_VALUE);
                        REQUIRE(settings[testKey5].get({key}, DEFAULT_VALUE) == DEFAULT_VALUE);
                    }

                    REQUIRE(settings.get({testKey6}, DEFAULT_VALUE) == SettingsNode::SettingsValues(testValue6));
                    for(const auto& key : testValue6) {
                        REQUIRE(settings.get({testKey6, key}, DEFAULT_VALUE) == DEFAULT_VALUE);
                        REQUIRE(settings[testKey6].get({key}, DEFAULT_VALUE) == DEFAULT_VALUE);
                    }
                }
            }
        }
    }

    SCENARIO("Addition of multiple key values", "[config][settingsNode]") {
        GIVEN("A basic setup") {
            const SettingsNode::SettingsKey rootKey("root-key");
            const SettingsNode::SettingsKeys key1({"key1a", "key1b", "key1c", "key1d"});
            const SettingsNode::SettingsValues value1({"value1a", "value1b", "value1c"});
            SettingsNode settings(rootKey);

            WHEN("We add the values") {
                REQUIRE(settings.add(key1, value1));

                THEN("It should contain it") {
                    SettingsNode::SettingsKeys searchKeys;
                    for(auto key : key1) {
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
                    REQUIRE(stageSettings->values() == value1);
                }
                THEN("We should get the values") {
                    REQUIRE(settings.get(key1) != boost::none);
                    REQUIRE(settings.get(key1).get() == value1);
                }
                THEN("We should get the values, not the default ones") {
                    const SettingsNode::SettingsValues DEFAULT_VALUE({"blaat"});

                    REQUIRE(settings.get(key1, DEFAULT_VALUE) == value1);
                }
            }
        }
        GIVEN("A settingsnode with a deep hierarchy") {
            SettingsNode::SettingsKeys veryLongKeys1;
            const SettingsNode::SettingsValues value1({"value1a", "value1b"});
            SettingsNode::SettingsKeys veryLongKeys2;
            const SettingsNode::SettingsValues value2({"value2a", "value2b"});
            SettingsNode settings("root-key");

            const size_t TEST_SIZE_DEPTH=8192U;

            for(size_t i = 0; i < TEST_SIZE_DEPTH; ++i) {
                veryLongKeys1.push_back("key1-" + to_string(i));
                veryLongKeys2.push_back("key2-" + to_string(i));
            }

            WHEN("We add the keys") {
                REQUIRE(settings.add(veryLongKeys1, value1));
                REQUIRE(settings.add(veryLongKeys2, value2));

                THEN("It should contain these") {
                    REQUIRE(settings.get(veryLongKeys1) != boost::none);
                    REQUIRE(settings.get(veryLongKeys1).get() == value1);

                    REQUIRE(settings.get(veryLongKeys2) != boost::none);
                    REQUIRE(settings.get(veryLongKeys2).get() == value2);
                }
            }
        }
        GIVEN("A settingsnode with a broad hierarchy") {
            const SettingsNode::SettingsKeys key1({"key1a", "key1b"});
            const SettingsNode::SettingsKeys key2({"key2a", "key2b"});
            SettingsNode settings("root-key");

            const size_t TEST_SIZE_BREADTH=8192U;

            WHEN("We add the keys") {
                SettingsNode::SettingsValues value1;
                SettingsNode::SettingsValues value2;

                for(size_t i = 0; i < TEST_SIZE_BREADTH; ++i) {
                    const SettingsNode::SettingsValue newValue1 = "value1-" + to_string(i);
                    const SettingsNode::SettingsValue newValue2 = "value2-" + to_string(i);

                    REQUIRE(settings.add(key1, newValue1));
                    REQUIRE(settings.add(key2, newValue2));

                    value1.push_back(newValue1);
                    value2.push_back(newValue2);
                }

                THEN("It should contain these") {
                    REQUIRE(settings.get(key1) != boost::none);
                    REQUIRE(settings.get(key1).get() == value1);

                    REQUIRE(settings.get(key2) != boost::none);
                    REQUIRE(settings.get(key2).get() == value2);
                }
            }
            WHEN("We add the keys as a whole") {
                SettingsNode::SettingsValues value1;
                SettingsNode::SettingsValues value2;

                for(size_t i = 0; i < TEST_SIZE_BREADTH; ++i) {
                    const SettingsNode::SettingsValue newValue1 = "value1-" + to_string(i);
                    const SettingsNode::SettingsValue newValue2 = "value2-" + to_string(i);

                    value1.push_back(newValue1);
                    value2.push_back(newValue2);
                }

                REQUIRE(settings.add(key1, value1));
                REQUIRE(settings.add(key2, value2));

                THEN("It should contain these") {
                    REQUIRE(settings.get(key1) != boost::none);
                    REQUIRE(settings.get(key1).get() == value1);

                    REQUIRE(settings.get(key2) != boost::none);
                    REQUIRE(settings.get(key2).get() == value2);
                }
            }
        }
    }

    SCENARIO("Testing the (in)equality operator", "[config][settingsNode]") {
        GIVEN("A setup settings node") {
            const SettingsNode::SettingsKey rootKey("root-key");
            const SettingsNode::SettingsKeys key1({"key1a", "key1b", "key1c", "key1d"});
            const SettingsNode::SettingsValues value1({"value1a", "value1b", "value1c"});
            const SettingsNode::SettingsKeys key2({"key2a", "key2b", "key2c", "key2d", "key2e", "key2f"});
            const SettingsNode::SettingsValues value2({"value2a", "value2b", "value2c", "value2d"});

            SettingsNode settings(rootKey);
            settings.add(key1, value1);
            settings.add(key2, value2);

            WHEN("We create a similar settings node") {
                SettingsNode similarSettings(rootKey);
                similarSettings.add(key1, value1);
                similarSettings.add(key2, value2);

                THEN("They should be equal") {
                    REQUIRE(settings == similarSettings);
                    REQUIRE_FALSE(settings != similarSettings);
                }
            }
            WHEN("We create a similar settings node with a different key") {
                SettingsNode similarSettings("other-root-key");
                similarSettings.add(key1, value1);
                similarSettings.add(key2, value2);

                THEN("They should not be equal") {
                    REQUIRE_FALSE(settings == similarSettings);
                    REQUIRE(settings != similarSettings);
                }
            }
            WHEN("We create a similar settings node with a different value") {
                SettingsNode similarSettings("root-key");
                similarSettings.add(key1, value2);

                THEN("They should not be equal") {
                    REQUIRE_FALSE(settings == similarSettings);
                    REQUIRE(settings != similarSettings);
                }
            }
            WHEN("We create an almost similar settings node") {
                SettingsNode similarSettings(rootKey);
                similarSettings.add(key1, value1);

                THEN("They should not be equal") {
                    REQUIRE_FALSE(settings == similarSettings);
                    REQUIRE(settings != similarSettings);
                }
            }
            WHEN("We create an almost similar settings node") {
                SettingsNode similarSettings(rootKey);
                similarSettings.add(key2, value2);

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
            const SettingsNode::SettingsKey rootKey("root-key");
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

    SCENARIO("Testing the copy and move constructor", "[config][settingsNode]") {
        GIVEN("A settings node to copy and move") {
            const SettingsNode::SettingsKey rootKey("root-key");
            const SettingsNode::SettingsKeys key1({"key1"});
            const SettingsNode::SettingsValues value1({"value1a", "value1b"});
            const SettingsNode::SettingsKeys key2({"key2"});
            const SettingsNode::SettingsValues value2({"value2a", "value2b"});
            const SettingsNode::SettingsKeys key3({"key3"});
            const SettingsNode::SettingsValues value3({"value3"}); 

            SettingsNode settings(rootKey);
            settings.add(key1, value1);
            settings.add(key2, value2);
            settings.add(key3, value3);

            WHEN("We copy the settings") {
                SettingsNode copy(settings);    // NOLINT(performance-unnecessary-copy-initialization)

                THEN("We should find back the values") {
                    REQUIRE(copy.key() == rootKey);
                    REQUIRE(copy.get(key1) != boost::none);
                    REQUIRE(copy.get(key1).get() == value1);
                    REQUIRE(copy.get(key2) != boost::none);
                    REQUIRE(copy.get(key2).get() == value2);
                    REQUIRE(copy.get(key3) != boost::none);
                    REQUIRE(copy.get(key3).get() == value3);
                }
                THEN("They should compare equal") {
                    REQUIRE(copy == settings);
                }
            }
            WHEN("We move the settings") {
                SettingsNode move(std::move(settings));

                THEN("We should find back the values") {
                    REQUIRE(move.key() == rootKey);
                    REQUIRE(move.get(key1) != boost::none);
                    REQUIRE(move.get(key1).get() == value1);
                    REQUIRE(move.get(key2) != boost::none);
                    REQUIRE(move.get(key2).get() == value2);
                    REQUIRE(move.get(key3) != boost::none);
                    REQUIRE(move.get(key3).get() == value3);
                }
            }
        }
    }

    SCENARIO("Testing the removal of values", "[config][settingsNode]") {
        GIVEN("A settings node setup with some values") {
            const SettingsNode::SettingsKey rootKey("root-key");
            const SettingsNode::SettingsKeys key({"key1"});
            const SettingsNode::SettingsValues value1({"value1a", "value1b"});
            const SettingsNode::SettingsValues value2({"value2a", "value2b"});
            const SettingsNode::SettingsValues value3({"value3"}); 

            SettingsNode settings(rootKey);
            settings.add(key, value1);
            settings.add(key, value2);
            settings.add(key, value3);

            WHEN("We clear the values") {
                bool returnCode = settings.clear(key);

                THEN("It should succeed") {
                    REQUIRE(returnCode);
                }

                THEN("They should be cleared") {
                    const SettingsNode::SettingsValues DEFAULT_VALUE({"blaat"});
                    REQUIRE(settings.get(key, DEFAULT_VALUE) == DEFAULT_VALUE);
                }
            }
        }
    }
} // namespace test
} // namespace config
} // namespace execHelper
