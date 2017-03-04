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
        for(const SettingsNode& value : m_values) {
            if(findKey == value.m_key) {
                return true;
            }
        }
        return false;
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
        if(m_key != other.m_key) {
            return false;
        }

        // Note: the order of the values may differ
        if(m_values.size() != other.m_values.size()) {
            return false;
        }
        for(const auto& value : m_values) {
            if(! other.contains(value.m_key)) {
                return false;
            }
            if(value != other[value.m_key]) {
                return false;
            }
        }
        return true;
    }

    bool SettingsNode::operator!=(const SettingsNode& other) const noexcept {
        return !(*this == other);
    }

    std::ostream& operator<<( std::ostream& os, const SettingsNode& settings) noexcept {
        os << settings.m_key << ": " << std::endl;
        for(const auto& value : settings.m_values) {
            os << "{" << value << "};" << std::endl;
        }
        return os;
    }
} }


