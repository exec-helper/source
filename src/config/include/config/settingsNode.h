#ifndef __SETTINGS_NODE_H__
#define __SETTINGS_NODE_H__

#include <memory>
#include <string>
#include <vector>

#include <boost/optional/optional.hpp>

namespace execHelper {
    namespace config {
        class SettingsNode {
            public:
                typedef std::string SettingsKey;
                typedef std::vector<SettingsKey> SettingsKeys;
                typedef SettingsKey SettingsValue;
                typedef std::vector<SettingsValue> SettingsValues; 

                SettingsNode(SettingsKey key) noexcept;
                SettingsNode(const SettingsNode& other) noexcept;
                SettingsNode(SettingsNode&& other) noexcept;
                ~SettingsNode() noexcept;

                SettingsNode& operator=(const SettingsNode& other) noexcept;
                bool operator==(const SettingsNode& other) const noexcept;
                bool operator!=(const SettingsNode& other) const noexcept;
                
                // Only allow const SettingsNode as return type
                // It is only valid to call this function if contains(key) == true
                const SettingsNode& operator[](const SettingsKey& key) const noexcept;

                SettingsKey key() const noexcept;
                SettingsValues values() const noexcept;

                bool contains(const SettingsKey& key) const noexcept;
                bool contains(const SettingsKeys& keys) const noexcept;

                boost::optional<SettingsValues> get(const SettingsKeys& keys) const noexcept;
                SettingsValues get(const SettingsKeys& keys, const SettingsValues& defaultValue) const noexcept;

                bool add(const SettingsValue& newValue) noexcept;
                bool add(const SettingsKeys& keys, const SettingsValue& newValue) noexcept;
                bool add(const std::initializer_list<SettingsValue>& newValues) noexcept;
                bool add(const SettingsKeys& keys, const std::initializer_list<SettingsValue>& newValues) noexcept;
                bool add(const SettingsValues& newValues) noexcept;
                bool add(const SettingsKeys& keys, const SettingsValues& newValues) noexcept;

                bool clear(const SettingsKey& key) noexcept;
                bool clear(const SettingsKeys& keys) noexcept;

            private:
                typedef std::vector<SettingsNode> SettingsNodeCollection;

                void deepCopy(const SettingsNode& other) noexcept;
                SettingsNode* at(const SettingsKey& key) noexcept;
                const SettingsNode* at(const SettingsKey& key) const noexcept;

                SettingsKey m_key;
                std::unique_ptr<SettingsNodeCollection> m_values;
        };

        std::ostream& operator<<( std::ostream& os, const SettingsNode& settings) noexcept;
    }
}

#endif /* __SETTINGS_NODE_H__ */
