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
                    static T getValue(config::SettingsNode::SettingsCollection& collection) noexcept {
                        return collection;
                    }
            };

            template<typename T>
            class ConfigValueImpl<T, false> {
                public:
                    static T getValue(config::SettingsNode::SettingsCollection& collection) noexcept {
                        return collection.back();
                    }
            };
        }

        template<typename T>
        class ConfigValue {
            public:
                static boost::optional<T> getSetting(const std::string& configKey, const core::Command& command, const config::SettingsNode& rootSettings) noexcept {
                    const config::SettingsNode& settings = getContainingSettings(command, rootSettings, configKey);
                    if(!settings.contains(configKey)) {
                        return boost::none;
                    }
                    config::SettingsNode::SettingsCollection collection = settings[configKey].toStringCollection();
                        return detail::ConfigValueImpl<T, isContainer<T>::value>::getValue(collection);
                }

                static T get(const std::string& configKey, const T& defaultValue, const core::Command& command, const config::SettingsNode& rootSettings) noexcept {
                    auto optionalValue = getSetting(configKey, command, rootSettings);
                    if(optionalValue == boost::none) {
                        return defaultValue;
                    }
                    return optionalValue.get();
                }
        };
    }
}

#endif  /* __CONFIG_VALUE_H__ */
