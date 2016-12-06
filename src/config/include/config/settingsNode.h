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

            ~SettingsNode();

            SettingsNode& operator[](const std::string& requestedKey) noexcept;
            const SettingsNode& operator[](const std::string& requestedKey) const noexcept ;
            bool contains(const std::string& findKey) const noexcept;
            std::vector<std::string> toStringCollection() const noexcept;
            bool operator==(const SettingsNode& other) const noexcept;
            bool operator!=(const SettingsNode& other) const noexcept;
        };

        std::ostream& operator<<( std::ostream& os, const SettingsNode& settings) noexcept;
    }
}

#endif /* __SETTINGS_NODE_H__ */
