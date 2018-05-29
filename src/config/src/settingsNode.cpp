#include "settingsNode.h"

#include <iostream>
#include <memory>
#include <ostream>

#include <log/assertions.h>

#include "logger.h"

using std::initializer_list;
using std::make_unique;
using std::ostream;
using std::string;

namespace {
ostream& stream(ostream& os, const execHelper::config::SettingsNode& settings,
                const string& prepend) noexcept {
    os << prepend << "- " << settings.key();
    if(!settings.values().empty()) {
        os << ":";
    }
    os << std::endl;
    const string newPrepend = string(prepend).append("  ");
    for(const auto& value : settings.values()) {
        stream(os, settings[value], newPrepend);
    }
    return os;
}
} // namespace

namespace execHelper {
namespace config {
SettingsNode::SettingsNode(SettingsKey key) noexcept : m_key(std::move(key)) {
    ;
}

SettingsNode::SettingsNode(const SettingsNode& other) noexcept
    : m_key(other.m_key) {
    deepCopy(other);
}

SettingsNode::SettingsNode(SettingsNode&& other) noexcept
    : m_key(std::move(other.m_key)), m_values(std::move(other.m_values)) {
    ;
}

SettingsNode::~SettingsNode() noexcept { ; }

SettingsNode& SettingsNode::operator=(const SettingsNode& other) noexcept {
    m_key = other.m_key;
    deepCopy(other);
    return *this;
}

SettingsNode& SettingsNode::operator=(SettingsNode&& other) noexcept {
    swap(other);
    return *this;
}

bool SettingsNode::operator==(const SettingsNode& other) const noexcept {
    if(m_key != other.m_key) {
        return false;
    }
    if(!m_values || !other.m_values) {
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

SettingsNode& SettingsNode::operator[](const SettingsKey& key) noexcept {
    if(contains(key)) {
        for(auto& value : *m_values) {
            if(value.m_key == key) {
                return value;
            }
        }
    }
    add(key);
    return m_values->back();
}

const SettingsNode& SettingsNode::operator[](const SettingsKey& key) const
    noexcept {
    expectsMessage(contains(key), "Key must exist");
    for(const auto& value : *m_values) {
        if(value.m_key == key) {
            return value;
        }
    }
    ensures(false);
    return *this;
}

bool SettingsNode::contains(const SettingsKey& key) const noexcept {
    if(!m_values) {
        return false;
    }
    for(const auto& value : *m_values) {
        if(value.m_key == key) {
            return true;
        }
    }
    return false;
}

bool SettingsNode::contains(const SettingsKeys& key) const noexcept {
    const SettingsNode* settings = this;
    for(const auto& keyPart : key) {
        if(settings->contains(keyPart)) {
            settings = settings->at(keyPart);
        } else {
            return false;
        }
    }
    return true;
}

bool SettingsNode::add(const SettingsValue& newValue) noexcept {
    if(!m_values) {
        m_values = make_unique<SettingsNodeCollection>();
    }
    m_values->emplace_back(SettingsNode(newValue));
    return true;
}

bool SettingsNode::add(const SettingsKeys& key,
                       const SettingsValue& newValue) noexcept {
    return add(key, SettingsValues({newValue}));
}

bool SettingsNode::add(
    const initializer_list<SettingsValue>& newValue) noexcept {
    return add(SettingsValues(newValue));
}

bool SettingsNode::add(
    const SettingsKeys& key,
    const initializer_list<SettingsValue>& newValue) noexcept {
    return add(key, SettingsValues(newValue));
}

bool SettingsNode::add(
    const std::initializer_list<SettingsKey>& key,
    const std::initializer_list<SettingsValue>& newValue) noexcept {
    return add(SettingsKeys(key), SettingsValues(newValue));
}

bool SettingsNode::add(const std::initializer_list<SettingsKey>& key,
                       const SettingsValues& newValue) noexcept {
    return add(SettingsKeys(key), newValue);
}

bool SettingsNode::add(const std::initializer_list<SettingsKey>& key,
                       const SettingsValue& newValue) noexcept {
    return add(SettingsKeys(key), SettingsValues({newValue}));
}

bool SettingsNode::add(const SettingsKeys& key,
                       const SettingsValues& newValue) noexcept {
    SettingsNode* settings = this;
    for(const auto& parentKey : key) {
        if(!settings->contains(parentKey)) {
            settings->add(parentKey);
        }
        settings = settings->at(parentKey);
    }
    bool retCode = settings->add(newValue);
    return retCode;
}

bool SettingsNode::add(const SettingsKey& key,
                       const SettingsValue& newValue) noexcept {
    return add(SettingsKeys({key}), {newValue});
}

bool SettingsNode::add(const SettingsValues& newValue) noexcept {
    if(!m_values) {
        m_values = make_unique<SettingsNodeCollection>();
    }
    m_values->reserve(m_values->size() + newValue.size());
    for(const auto& valueToAdd : newValue) {
        m_values->emplace_back(SettingsNode(valueToAdd));
    }
    return true;
}

bool SettingsNode::add(const SettingsKey& key,
                       const SettingsValues& newValue) noexcept {
    return add(SettingsKeys({key}), newValue);
}

bool SettingsNode::clear(const SettingsKey& key) noexcept {
    return clear(SettingsKeys({key}));
}

bool SettingsNode::clear(const SettingsKeys& keys) noexcept {
    if(keys.empty()) {
        LOG(debug) << "Cannot clear the settingsnode itself";
        return false;
    }
    if(!contains(keys)) {
        return true;
    }

    SettingsNode* settings = this;
    for(auto key = keys.begin(); key != keys.end() - 1; ++key) {
        settings = settings->at(*key);
    }
    for(auto value = settings->m_values->begin();
        value != settings->m_values->end(); ++value) {
        if(value->m_key == keys.back()) {
            settings->m_values->erase(value);
            return true;
        }
    }
    return false;
}

SettingsValues SettingsNode::values() const noexcept {
    if(!m_values) {
        return SettingsKeys();
    }
    SettingsValues result;
    for(const auto& value : *m_values) {
        result.push_back(value.m_key);
    }
    return result;
}

SettingsKey SettingsNode::key() const noexcept { return m_key; }

void SettingsNode::swap(SettingsNode& other) noexcept {
    m_key.swap(other.m_key);
    m_values.swap(other.m_values);
}

void SettingsNode::overwrite(const SettingsNode& newSettings) noexcept {
    for(const auto& key : newSettings.values()) {
        const SettingsNode& newValue = newSettings[key];
        if(!contains(key)) {
            add(key);
        }
        at(key)->deepCopy(newValue);
    }
}

SettingsNode* SettingsNode::at(const SettingsKey& key) noexcept {
    expectsMessage(contains(key), "Key must exist");
    for(SettingsNode& value : *m_values) {
        if(value.m_key == key) {
            return &value;
        }
    }
    ensures(false);
    return this;
}

const SettingsNode* SettingsNode::at(const SettingsKey& key) const noexcept {
    expectsMessage(contains(key), "Key must exist");
    for(const SettingsNode& value : *m_values) {
        if(value.m_key == key) {
            return &value;
        }
    }
    ensures(false);
    return this;
}

SettingsNode* SettingsNode::at(const SettingsKeys& key) noexcept {
    expectsMessage(contains(key), "Key must exist");
    SettingsNode* settings = this;
    for(const auto& keyPart : key) {
        settings = settings->at(keyPart);
    }
    return settings;
}

const SettingsNode* SettingsNode::at(const SettingsKeys& key) const noexcept {
    expectsMessage(contains(key), "Key must exist");
    const SettingsNode* settings = this;
    for(const auto& keyPart : key) {
        settings = settings->at(keyPart);
    }
    return settings;
}

void SettingsNode::deepCopy(const SettingsNode& other) noexcept {
    if(!other.m_values) {
        return;
    }
    m_values = make_unique<SettingsNodeCollection>();

    SettingsValues otherValues = other.values();
    m_values->reserve(otherValues.size());
    for(const auto& otherKey : otherValues) {
        SettingsNode newNode = other[otherKey];
        m_values->emplace_back(newNode);
    }
}

ostream& operator<<(ostream& os, const SettingsNode& settings) noexcept {
    os << settings.key();
    if(!settings.values().empty()) {
        os << ":";
    }
    os << std::endl;
    for(const auto& value : settings.values()) {
        stream(os, settings[value], "  ");
    }
    return os;
}
} // namespace config
} // namespace execHelper
