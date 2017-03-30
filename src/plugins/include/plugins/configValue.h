#ifndef __CONFIG_VALUE_H__
#define __CONFIG_VALUE_H__

#include <vector>

#include <boost/optional/optional.hpp>

#include "config/settingsNode.h"
#include "core/options.h"

#include "pluginUtils.h"

namespace execHelper {
    namespace plugins {
        template<class T>
        struct isContainer : public std::false_type {};

        template<class T, class Alloc>
        struct isContainer<std::vector<T, Alloc>> : public std::true_type {};

        namespace detail {
            template<typename T, bool isCollection>
            class ConfigValueImpl {
                public:
                    static boost::optional<T> getValue(config::SettingsNode::SettingsValues& collection) noexcept {
                        return collection;
                    }
            };

            template<typename T>
            class ConfigValueImpl<T, false> {
                public:
                    static boost::optional<T> getValue(config::SettingsNode::SettingsValues& collection) noexcept {
                        if(collection.empty()) {
                            return boost::none;
                        }
                        return collection.back();
                    }
            };
        }

        template<typename T>
        class ConfigValue {
            public:
                typedef std::initializer_list<std::vector<std::string>> OrderedConfigKeys;

                static boost::optional<T> getSetting(const std::string& key, const config::SettingsNode& rootSettings, const OrderedConfigKeys& orderedConfigKeys) noexcept {
                    for(auto& configKeys : orderedConfigKeys) {
                        config::SettingsNode::SettingsKeys keys = configKeys;
                        keys.push_back(key);
                        boost::optional<config::SettingsNode::SettingsValues> values = rootSettings.get(keys);
                        if(values != boost::none) {
                            return detail::ConfigValueImpl<T, isContainer<T>::value>::getValue(values.get());
                        }
                    }
                    return boost::none;
                }

                static boost::optional<T> getSetting(const std::string& key, const config::SettingsNode& rootSettings, const core::Command& command) noexcept {
                    return getSetting(key, rootSettings, {{command}, {}});
                }

                static T get(const std::string& configKey, const T& defaultValue, const core::Command& command, const config::SettingsNode& rootSettings) noexcept {
                    return get(configKey, defaultValue, rootSettings, {{command}, {}});
                }

                static T get(const std::string& configKey, const T& defaultValue, const config::SettingsNode& rootSettings, const OrderedConfigKeys& orderedConfigKeys) noexcept {
                    auto configValue = getSetting(configKey, rootSettings, orderedConfigKeys);
                    if(configValue != boost::none) {
                        return configValue.get();
                    }
                    return defaultValue;
                }
        };

        template<>
        class ConfigValue<const config::SettingsNode&> {
            public:
                typedef std::initializer_list<std::vector<std::string>> OrderedConfigKeys;

                static boost::optional<const config::SettingsNode&> getSetting(const std::string& key, const config::SettingsNode& rootSettings, const OrderedConfigKeys& orderedConfigKeys) noexcept {
                    for(const auto& configKeys : orderedConfigKeys) {
                        config::SettingsNode::SettingsKeys keys = configKeys;
                        keys.push_back(key);
                        if(rootSettings.contains(keys)) {
                            const config::SettingsNode* settings = &rootSettings;
                            // We know that the whole key chain exists, so we do not need to check for this
                            for(const auto& configKey : configKeys) {
                                settings = &(*settings)[configKey];
                            }
                            return (*settings)[key];
                        }
                    }
                    return boost::none;
                }

                static boost::optional<const config::SettingsNode&> getSetting(const std::string& key, const config::SettingsNode& rootSettings, const core::Command& command) noexcept {
                    return getSetting(key, rootSettings, {{command}, {}});
                }

                static const config::SettingsNode& get(const std::string& configKey, const config::SettingsNode& defaultValue, const core::Command& command, const config::SettingsNode& rootSettings) noexcept {
                    return get(configKey, defaultValue, rootSettings, {{command}, {}});
                }

                static const config::SettingsNode& get(const std::string& configKey, const config::SettingsNode& defaultValue, const config::SettingsNode& rootSettings, const OrderedConfigKeys& orderedConfigKeys) noexcept {
                    auto configValue = getSetting(configKey, rootSettings, orderedConfigKeys);
                    if(configValue != boost::none) {
                        return configValue.get();
                    }
                    return defaultValue;
                }
        };
    }
}

#endif  /* __CONFIG_VALUE_H__ */
