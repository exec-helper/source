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
            std::vector<SettingsNode> values;

            ~SettingsNode() {
                ;
            }

            SettingsNode& operator[](const std::string& requestedKey) noexcept {
                const SettingsNode& node = *this;
                return const_cast<SettingsNode&>(node[requestedKey]);
            }

            const SettingsNode& operator[](const std::string& requestedKey) const noexcept {
                for(const auto& value : values) {
                    if(requestedKey == value.key) {
                        return value;
                    }
                }
                return *this;
            }

            std::vector<std::string> toStringCollection() const noexcept {
                std::vector<std::string> results;

                results.reserve(values.size());
                for(const auto& value : values) {
                    results.push_back(value.key);
                }
                return results;
            }

            bool operator==(const SettingsNode& other) const noexcept {
                return key == other.key && values == other.values;
            }

            bool operator!=(const SettingsNode& other) const noexcept {
                return !(*this == other);
            }
        };
    }
}

#endif /* __SETTINGS_NODE_H__ */
