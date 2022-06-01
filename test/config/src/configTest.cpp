#include <algorithm>
#include <string>

#include <gsl/gsl>

#include "config/config.h"
#include "config/pattern.h"

#include "base-utils/configFileWriter.h"
#include "unittest/catch.h"
#include "utils/utils.h"

using gsl::czstring;
using std::find;
using std::string;
using std::to_string;

using execHelper::config::SettingsNode;

using execHelper::test::baseUtils::ConfigFileWriter;
using execHelper::test::utils::writeSettingsFile;

namespace execHelper::config::test {
SCENARIO("Parse properly written settings files", "[config][config-config]") {
    MAKE_COMBINATIONS("Of several settings") {
        SettingsNode settings("exec-helper");
        REQUIRE(settings.add("dummy-key", "dummy-value"));
        Patterns patterns;

        COMBINATIONS("Add multiple keys") {
            const unsigned int SIZE = 100U;

            for(auto i = 0U; i < SIZE; ++i) {
                SettingsKey key("key-value-key");
                key.append(to_string(i));

                SettingsValue value("key-value-value");
                value.append(to_string(i));
                REQUIRE(settings.add(key, value));
            }
        }

        COMBINATIONS("Add multiple arrays") {
            const unsigned int KEY_SIZE = 20U;
            const unsigned int VALUE_SIZE = KEY_SIZE;

            for(auto i = 0U; i < KEY_SIZE; ++i) {
                SettingsKey key("map-key");
                key.append(to_string(i));

                SettingsValues values;
                values.reserve(VALUE_SIZE);
                for(auto j = 0U; j < VALUE_SIZE; ++j) {
                    SettingsValue newValue("map-value");
                    newValue.append(to_string(j));
                    values.emplace_back(newValue);
                }
                REQUIRE(settings.add(key, values));
            }
        }

        // COMBINATIONS("Add a big tree") {
        // const unsigned int SIZE = 4U;
        // const unsigned int VALUE_SIZE = SIZE;

        // SettingsKeys keys({"tree-key"});
        // for(auto i = 0U; i < SIZE; ++i) {
        // SettingsKey key("key");
        // key.append(to_string(i));
        // keys.emplace_back(key);

        // settings.add(keys, string("some-value").append(to_string(i)));
        // SettingsValues values;
        // for(auto j = 0U; j < VALUE_SIZE; ++j) {
        // SettingsValue newValue("value");
        // newValue.append(to_string(j));
        // values.emplace_back(newValue);
        //}
        // settings.add(keys, values);
        //}
        //}

        COMBINATIONS("Add a pattern") {
            patterns.emplace_back(Pattern("pattern1", {"value1a", "value1b"}));
        }

        COMBINATIONS("Add multiple patterns") {
            const unsigned int SIZE = 50U;
            for(auto i = 0U; i < SIZE; ++i) {
                PatternKey newKey("multiple-pattern");
                newKey.append(to_string(i));

                PatternValue newValue("multiple-pattern-value");
                newValue.append(to_string(i));

                patterns.emplace_back(
                    Pattern(newKey, {newValue}, std::nullopt, newKey));
            }
        }

        ConfigFileWriter configFile;
        writeSettingsFile(gsl::not_null<ConfigFileWriter*>(&configFile),
                          settings, patterns);
        auto parsedSettingsFile = parseSettingsFile(configFile.getPath());

        THEN_CHECK("It must succeed") {
            REQUIRE(parsedSettingsFile != std::nullopt);
        }

        THEN_CHECK("We must find the proper settings and patterns") {
            auto foundPatterns = parsedSettingsFile.value().first;
            auto foundSettings = parsedSettingsFile.value().second;

            REQUIRE(foundSettings == settings);

            // Patterns are vectors, but their order is not necessarily
            // preserved by the config reader and writer, so we can not simply
            // use the comparison operator to compare them.
            REQUIRE(foundPatterns.size() == patterns.size());
            for(const auto& pattern : patterns) {
                REQUIRE(find(foundPatterns.begin(), foundPatterns.end(),
                             pattern) != foundPatterns.end());
            }
        }
    }
}
} // namespace execHelper::config::test
