#ifndef ADD_TO_CONFIG_INCLUDE
#define ADD_TO_CONFIG_INCLUDE

#include <filesystem>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gsl/pointers>

#include "config/variablesMap.h"

#include "base-utils/nonEmptyString.h"

namespace execHelper::test {
inline void
addToConfig(const execHelper::config::SettingsKeys& key,
            const std::string& value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    if(!config->add(key, value)) {
        throw std::runtime_error("Failed to add key " + key.back() +
                                 " with value '" + value + "' to config");
    }
}

inline void
addToConfig(const execHelper::config::SettingsKeys& key,
            const std::string_view& value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    addToConfig(key, std::string(value), config);
}

inline void
addToConfig(const execHelper::config::SettingsKeys& key,
            const NonEmptyString& value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    if(!config->add(key, *value)) {
        throw std::runtime_error("Failed to add key " + key.back() +
                                 " with value '" + *value + "' to config");
    }
}

template <typename T>
inline void
addToConfig(const execHelper::config::SettingsKeys& key,
            const std::vector<T>& value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    std::for_each(value.begin(), value.end(), [&key, &config](const auto& element) {
        addToConfig(key, element, config);
    });
}

inline void
addToConfig(const execHelper::config::SettingsKeys& key, bool value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    if(value) {
        if(!config->add(key, "yes")) {
            throw std::runtime_error("Failed to add key " + key.back() +
                                     " with value 'true' to config");
        }
    }
    if(!config->add(key, "no")) {
        throw std::runtime_error("Failed to add key " + key.back() +
                                 " with value 'false' to config");
    }
}

inline void
addToConfig(const execHelper::config::SettingsKeys& key, std::uint16_t value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    addToConfig(key, std::to_string(value), config);
}

inline void
addToConfig(const execHelper::config::SettingsKeys& key, std::uint32_t value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    addToConfig(key, std::to_string(value), config);
}

inline void
addToConfig(execHelper::config::SettingsKeys key,
            const std::pair<std::string, std::string>& value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    key.push_back(value.first);
    if(!config->add(key, value.second)) {
        throw std::runtime_error("Failed to add key " + value.first +
                                 " with first value '" + value.second +
                                 "' to config");
    }
}

inline void
addToConfig(const execHelper::config::SettingsKeys& key,
            const std::filesystem::path& value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    addToConfig(key, value.string(), config);
}

template <typename K, typename V>
inline void
addToConfig(const execHelper::config::SettingsKey& key,
            const std::map<K, V>& value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    std::for_each(value.begin(), value.end(), [&key, &config](const auto& element) {
        addToConfig(execHelper::config::SettingsKeys({key}), element, config);
    });
}

template <typename T>
inline void
addToConfig(const execHelper::config::SettingsKeys& key,
            const std::optional<T>& value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    if(value) {
        addToConfig(key, *value, config);
    }
}

template <typename T>
inline void
addToConfig(const execHelper::config::SettingsKey& key, const T& value,
            gsl::not_null<execHelper::config::VariablesMap*> config) {
    addToConfig(execHelper::config::SettingsKeys({key}), value, config);
}
} // namespace execHelper::test

#endif /* ADD_TO_CONFIG_INCLUDE */
