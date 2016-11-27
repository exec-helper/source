#ifndef __SETTINGS_NODE_H__
#define __SETTINGS_NODE_H__

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace execHelper {
    namespace config {
        struct SettingsNode {
            std::string key;
            std::unique_ptr<std::vector<SettingsNode>> values;

            SettingsNode& operator[](const std::string& requestedKey) noexcept {
                const SettingsNode& node = *this;
                return const_cast<SettingsNode&>(node[requestedKey]);
            }

            const SettingsNode& operator[](const std::string& requestedKey) const noexcept {
                if(values) {
                    for(const auto& value : *values) {
                        if(requestedKey == value.key) {
                            return value;
                        }
                    }
                }
                return *this;
            }

            const SettingsNode& getValues(const std::initializer_list<std::string>& keys) const noexcept {
                const SettingsNode* node = this;
                for(const auto& requestedKey : keys) {
                    node = &(*node)[requestedKey];
                }
                return *node;
            }

            std::vector<std::string> toString() const noexcept {
                std::vector<std::string> results;

                if(values) {
                    results.reserve(values->size());
                    for(const auto& value : *values) {
                        results.push_back(value.key);
                    }
                }
                return results;
            }
        };

    }
}

#endif /* __SETTINGS_NODE_H__ */
