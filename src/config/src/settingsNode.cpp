#include "settingsNode.h"

#include <iostream>
#include <memory>
#include <optional>
#include <ostream>

#include <log/assertions.h>

#include "logger.h"

using std::initializer_list;
using std::make_unique;
using std::ostream;
using std::string;

using execHelper::config::SettingsKeys;
using execHelper::config::SettingsValues;

namespace {
ostream& stream(ostream& os, const execHelper::config::SettingsNode& settings,
                const string& prepend) noexcept {
    os << prepend << "- " << settings.key();
    auto values =
        settings.get<SettingsValues>(SettingsKeys(), SettingsValues());
    if(!values.empty()) {
        os << ":";
    }
    os << std::endl;
    const string newPrepend = string(prepend).append("  ");
    for(const auto& value : values) {
        stream(os, settings[value], newPrepend);
    }
    return os;
}
} // namespace

namespace execHelper::config {
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
    if(m_values->size() != other.m_values->size()) {
        return false;
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
        auto value =
            find_if(m_values->begin(), m_values->end(),
                    [&key](const auto& value) { return value.m_key == key; });
        ensures(value != m_values->end());
        return *value;
    }
    add(key);
    return m_values->back();
}

const SettingsNode& SettingsNode::operator[](const SettingsKey& key) const
    noexcept {
    expectsMessage(contains(key), "Key must exist");
    auto value =
        find_if(m_values->begin(), m_values->end(),
                [&key](const auto& value) { return value.m_key == key; });
    ensures(value != m_values->end());
    return *value;
}

bool SettingsNode::contains(const SettingsKey& key) const noexcept {
    if(!m_values) {
        return false;
    }
    return std::any_of(
        m_values->begin(), m_values->end(),
        [&key](const auto& value) { return value.m_key == key; });
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
    std::transform(newValue.begin(), newValue.end(),
                   std::back_inserter(*m_values),
                   [](auto&& value) { return SettingsNode(value); });
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

std::optional<SettingsValues> SettingsNode::values() const noexcept {
    if(!m_values) {
        return std::nullopt;
    }
    SettingsValues result;
    result.reserve(m_values->size());
    std::transform(m_values->begin(), m_values->end(),
                   std::back_inserter(result),
                   [](const auto& value) { return value.m_key; });
    return result;
}

SettingsKey SettingsNode::key() const noexcept { return m_key; }

void SettingsNode::swap(SettingsNode& other) noexcept {
    m_key.swap(other.m_key);
    m_values.swap(other.m_values);
}

void SettingsNode::overwrite(const SettingsNode& newSettings) noexcept {
    std::vector<std::string> children =
        newSettings.values().value_or(std::vector<std::string>());
    for(const auto& key : children) {
        const SettingsNode& newValue = newSettings[key];
        if(!contains(key)) {
            add(key);
        }
        at(key)->deepCopy(newValue);
    }
}

SettingsNode* SettingsNode::at(const SettingsKey& key) noexcept {
    expectsMessage(contains(key), "Key must exist");
    auto value =
        std::find_if(m_values->begin(), m_values->end(),
                     [&key](const auto& value) { return value.m_key == key; });
    ensures(value != m_values->end());
    return &(*value);
}

const SettingsNode* SettingsNode::at(const SettingsKey& key) const noexcept {
    expectsMessage(contains(key), "Key must exist");
    auto value =
        std::find_if(m_values->begin(), m_values->end(),
                     [&key](const auto& value) { return value.m_key == key; });
    ensures(value != m_values->end());
    return &(*value);
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
    m_values->reserve(other.m_values->size());
    std::copy(other.m_values->begin(), other.m_values->end(),
              std::back_inserter(*m_values));
}

ostream& operator<<(ostream& os, const SettingsNode& settings) noexcept {
    return stream(os, settings, "");
}
} // namespace execHelper::config
