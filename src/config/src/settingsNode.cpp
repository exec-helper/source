#include "settingsNode.h"

namespace execHelper { namespace config {
    SettingsNode::~SettingsNode() {
        ;
    }

    SettingsNode& SettingsNode::operator[](const SettingsKey& requestedKey) noexcept {
        const SettingsNode& node = *this;
        return const_cast<SettingsNode&>(node[requestedKey]);
    }

    const SettingsNode& SettingsNode::operator[](const SettingsKey& requestedKey) const noexcept {
        for(const SettingsNode& value : m_values) {
            if(requestedKey == value.m_key) {
                return value;
            }
        }
        return *this;
    }

    bool SettingsNode::contains(const SettingsKey& findKey) const noexcept {
        return this->operator[](findKey) != *this;
    }

    SettingsNode::SettingsCollection SettingsNode::toStringCollection() const noexcept {
        std::vector<SettingsKey> results;

        results.reserve(m_values.size());
        for(const auto& value : m_values) {
            results.push_back(value.m_key);
        }
        return results;
    }

    bool SettingsNode::operator==(const SettingsNode& other) const noexcept {
        return m_key == other.m_key && m_values == other.m_values;
    }

    bool SettingsNode::operator!=(const SettingsNode& other) const noexcept {
        return !(*this == other);
    }

    std::ostream& operator<<( std::ostream& os, const SettingsNode& settings) noexcept {
        os << settings.m_key << ": ";
        for(const auto& value : settings.m_values) {
            os << "{" << value << "};";
        }
        return os;
    }
} }


