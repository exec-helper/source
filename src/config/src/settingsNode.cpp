#include "settingsNode.h"

#include <iostream>
#include <ostream>

using std::string;
using std::initializer_list;

namespace execHelper {
    namespace config {
        SettingsNode::SettingsNode(SettingsKey key) noexcept :
            m_key(std::move(key))
        {
            ;
        }

        SettingsNode::SettingsNode(const SettingsNode& other) noexcept :
            m_key(other.m_key)
        {
            deepCopy(other);
        }

        SettingsNode::SettingsNode(SettingsNode&& other) noexcept :
            m_key(std::move(other.m_key)),
            m_values(std::move(other.m_values))
        {
            ;
        }

        SettingsNode::~SettingsNode() noexcept {
            ;
        }

        SettingsNode& SettingsNode::operator=(const SettingsNode& other) noexcept {
            m_key = other.m_key;
            deepCopy(other);
            return *this;
        }

        bool SettingsNode::operator==(const SettingsNode& other) const noexcept {
            if(m_key != other.m_key) {
                return false;
            }
            if(! m_values || !other.m_values) {
                return (!m_values && !other.m_values);
            }
            for(const auto& value : *m_values) {
                if(!other.contains(value.m_key)) {
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

        const SettingsNode& SettingsNode::operator[](const SettingsKey& key) const noexcept {
            assert(m_values);
            for(const auto& value : *m_values) {
                if(value.m_key == key) {
                    return value;
                }
            }
            assert(false);
            return *this;
        }

        bool SettingsNode::contains(const SettingsKey& key) const noexcept {
            if(! m_values) {
                return false;
            }
            for(const auto& value : *m_values) {
                if(value.m_key == key) {
                    return true;
                }
            }
            return false;
        }

        bool SettingsNode::contains(const SettingsKeys& keys) const noexcept {
            const SettingsNode* settings = this;
            for(const auto& key : keys) {
                if(settings->contains(key)) {
                    settings = settings->at(key);
                } else {
                    return false;
                }
            }
            return true;
        }

        boost::optional<SettingsNode::SettingsValues> SettingsNode::get(const SettingsKeys& keys) const noexcept {
            if(keys.empty() || !m_values) {
                return boost::none;
            }
            const SettingsNode* settings = this;
            for(const auto& key : keys) {
                if(!settings->contains(key)) {
                    return boost::none;
                }
                settings = &(*settings)[key];
            }
            if(! settings->m_values) {
                return boost::none;
            }
            return settings->values();
        }

        SettingsNode::SettingsValues SettingsNode::get(const SettingsKeys& keys, const SettingsValues& defaultValue) const noexcept {
            auto optionalValue = get(keys);
            if(optionalValue != boost::none) {
                return optionalValue.get();
            }
            return defaultValue;
        }

        bool SettingsNode::add(const SettingsValue& newValue) noexcept {
            if(!m_values) {
                m_values.reset(new SettingsNodeCollection());
            }
            m_values->push_back(newValue);
            return true;
        }

        bool SettingsNode::add(const SettingsKeys& keys, const SettingsValue& newValue) noexcept {
            return add(keys, {newValue});
        }

        bool SettingsNode::add(const initializer_list<SettingsValue>& newValues) noexcept {
            return add(SettingsValues(newValues));
        }

        bool SettingsNode::add(const SettingsKeys& keys, const initializer_list<SettingsValue>& newValues) noexcept {
            return add(keys, SettingsValues(newValues));
        }

        bool SettingsNode::add(const SettingsKeys& keys, const SettingsValues& newValues) noexcept {
            SettingsNode* settings = this;
            for(const auto& parentKey : keys) {
                if(!settings->contains(parentKey)) {
                    settings->add(parentKey);
                }
                settings = settings->at(parentKey);
            }
            bool retCode = settings->add(newValues);
            return retCode;
        }

        bool SettingsNode::add(const SettingsValues& newValues) noexcept {
            if(!m_values) {
                m_values.reset(new SettingsNodeCollection());
            }
            m_values->insert(m_values->end(), newValues.begin(), newValues.end());
            return true;
        }

        bool SettingsNode::clear(const SettingsKey& key) noexcept {
            return clear(SettingsKeys({key}));
        }

        bool SettingsNode::clear(const SettingsKeys& keys) noexcept {
            if(! contains(keys)) {
                return true;
            }

            SettingsNode* settings = this;
            for(const auto& key : keys) {
                settings = settings->at(key);
            }
            settings->m_values.reset();
            return true;
        }

        SettingsNode::SettingsValues SettingsNode::values() const noexcept {
            if(! m_values) {
                return SettingsKeys();
            }
            SettingsValues result;
            for(const auto& value : *m_values) {
                result.push_back(value.m_key);
            }
            return result;
        }

        SettingsNode::SettingsKey SettingsNode::key() const noexcept {
            return m_key;
        }

        SettingsNode* SettingsNode::at(const SettingsKey& key) noexcept {
            for(SettingsNode& value : *m_values) {
                if(value.m_key == key) {
                    return &value;
                }
            }
            assert(false);
            return this;
        }

        const SettingsNode* SettingsNode::at(const SettingsKey& key) const noexcept {
            for(const SettingsNode& value : *m_values) {
                if(value.m_key == key) {
                    return &value;
                }
            }
            assert(false);
            return this;
        }

        void SettingsNode::deepCopy(const SettingsNode& other) noexcept {
            if(!other.m_values) {
                return;
            }
            m_values.reset(new SettingsNodeCollection());

            SettingsNode::SettingsValues otherValues = other.values();
            m_values->reserve(otherValues.size());
            for(const auto& otherKey : otherValues) {
                SettingsNode newNode = other[otherKey]; 
                m_values->emplace_back(newNode);
            }
        }

        std::ostream& operator<<( std::ostream& os, const SettingsNode& settings) noexcept {
            os << settings.key() << ": " << std::endl;
            for(const auto& value : settings.values()) {
                os << "{" << value << "};" << std::endl;
            }
            return os;
        }
    } // config
} // execHelper
