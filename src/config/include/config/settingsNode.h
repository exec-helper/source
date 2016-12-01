#ifndef __SETTINGS_NODE_H__
#define __SETTINGS_NODE_H__

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

namespace execHelper {
    namespace config {
        struct SettingsNode {
            std::string m_key;
            std::vector<SettingsNode> m_values;

            ~SettingsNode() {
                ;
            }

            SettingsNode& operator[](const std::string& requestedKey) noexcept {
                const SettingsNode& node = *this;
                return const_cast<SettingsNode&>(node[requestedKey]);
            }

            const SettingsNode& operator[](const std::string& requestedKey) const noexcept {
                for(const SettingsNode& value : m_values) {
                    if(requestedKey == value.m_key) {
                        return value;
                    }
                }
                return *this;
            }

            bool contains(const std::string& findKey) const noexcept {
                return this->operator[](findKey) != *this;
            }

            std::vector<std::string> toStringCollection() const noexcept {
                std::vector<std::string> results;

                results.reserve(m_values.size());
                for(const auto& value : m_values) {
                    results.push_back(value.m_key);
                }
                return results;
            }

            bool operator==(const SettingsNode& other) const noexcept {
                return m_key == other.m_key && m_values == other.m_values;
            }

            bool operator!=(const SettingsNode& other) const noexcept {
                return !(*this == other);
            }
        };
    }
}

#endif /* __SETTINGS_NODE_H__ */
