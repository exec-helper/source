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
auto stream(ostream& os, const execHelper::config::SettingsNode& settings,
            const string& prepend) noexcept -> ostream& {
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

auto SettingsNode::operator=(const SettingsNode& other) noexcept
    -> SettingsNode& {
    if(this != &other) {
        m_key = other.m_key;
        deepCopy(other);
    }
    return *this;
}

auto SettingsNode::operator=(SettingsNode&& other) noexcept -> SettingsNode& {
    swap(other);
    return *this;
}

auto SettingsNode::operator==(const SettingsNode& other) const noexcept
    -> bool {
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

auto SettingsNode::operator!=(const SettingsNode& other) const noexcept
    -> bool {
    return !(*this == other);
}

auto SettingsNode::operator[](const SettingsKey& key) noexcept
    -> SettingsNode& {
    if(contains(key)) {
        auto value =
            find_if(m_values->begin(), m_values->end(),
                    [&key](const auto& value) { return value.m_key == key; });
        ensures(value != m_values->end());
        return *value;
    }
    if(!add(key)) {
        LOG(warning) << "Failed to add key '" << key << "'";
    }
    return m_values->back();
}

auto SettingsNode::operator[](const SettingsKey& key) const noexcept
    -> const SettingsNode& {
    expectsMessage(contains(key), "Key must exist");
    auto value =
        find_if(m_values->begin(), m_values->end(),
                [&key](const auto& value) { return value.m_key == key; });
    ensures(value != m_values->end());
    return *value;
}

auto SettingsNode::contains(const SettingsKey& key) const noexcept -> bool {
    if(!m_values) {
        return false;
    }
    return std::any_of(
        m_values->begin(), m_values->end(),
        [&key](const auto& value) { return value.m_key == key; });
}

auto SettingsNode::contains(const SettingsKeys& key) const noexcept -> bool {
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

auto SettingsNode::add(const SettingsValue& newValue) noexcept -> bool {
    if(!m_values) {
        m_values = make_unique<SettingsNodeCollection>();
    }
    m_values->emplace_back(SettingsNode(newValue));
    return true;
}

auto SettingsNode::add(const SettingsKeys& key,
                       const SettingsValue& newValue) noexcept -> bool {
    return add(key, SettingsValues({newValue}));
}

auto SettingsNode::add(const initializer_list<SettingsValue>& newValue) noexcept
    -> bool {
    return add(SettingsValues(newValue));
}

auto SettingsNode::add(const SettingsKeys& key,
                       const initializer_list<SettingsValue>& newValue) noexcept
    -> bool {
    return add(key, SettingsValues(newValue));
}

auto SettingsNode::add(
    const std::initializer_list<SettingsKey>& key,
    const std::initializer_list<SettingsValue>& newValue) noexcept -> bool {
    return add(SettingsKeys(key), SettingsValues(newValue));
}

auto SettingsNode::add(const std::initializer_list<SettingsKey>& key,
                       const SettingsValues& newValue) noexcept -> bool {
    return add(SettingsKeys(key), newValue);
}

auto SettingsNode::add(const std::initializer_list<SettingsKey>& key,
                       const SettingsValue& newValue) noexcept -> bool {
    return add(SettingsKeys(key), SettingsValues({newValue}));
}

auto SettingsNode::add(const SettingsKeys& key,
                       const SettingsValues& newValue) noexcept -> bool {
    SettingsNode* settings = this;
    for(const auto& parentKey : key) {
        if(!settings->contains(parentKey)) {
            if(!settings->add(parentKey)) {
                LOG(warning) << "Failed to add key '" << parentKey << "'";
            }
        }
        settings = settings->at(parentKey);
    }
    return settings->add(newValue);
}

auto SettingsNode::add(const SettingsKey& key,
                       const SettingsValue& newValue) noexcept -> bool {
    return add(SettingsKeys({key}), {newValue});
}

auto SettingsNode::add(const SettingsValues& newValue) noexcept -> bool {
    if(!m_values) {
        m_values = make_unique<SettingsNodeCollection>();
    }
    m_values->reserve(m_values->size() + newValue.size());
    std::transform(newValue.begin(), newValue.end(),
                   std::back_inserter(*m_values),
                   [](auto&& value) { return SettingsNode(value); });
    return true;
}

auto SettingsNode::add(const SettingsKey& key,
                       const SettingsValues& newValue) noexcept -> bool {
    return add(SettingsKeys({key}), newValue);
}

auto SettingsNode::clear(const SettingsKey& key) noexcept -> bool {
    return clear(SettingsKeys({key}));
}

auto SettingsNode::clear(const SettingsKeys& keys) noexcept -> bool {
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

auto SettingsNode::values() const noexcept -> std::optional<SettingsValues> {
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

auto SettingsNode::key() const noexcept -> SettingsKey { return m_key; }

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
            if(!add(key)) {
                LOG(warning) << "Failed to add key '" << key << "'";
            }
        }
        at(key)->deepCopy(newValue);
    }
}

auto SettingsNode::at(const SettingsKey& key) noexcept -> SettingsNode* {
    expectsMessage(contains(key), "Key must exist");
    auto value =
        std::find_if(m_values->begin(), m_values->end(),
                     [&key](const auto& value) { return value.m_key == key; });
    ensures(value != m_values->end());
    return &(*value);
}

auto SettingsNode::at(const SettingsKey& key) const noexcept
    -> const SettingsNode* {
    expectsMessage(contains(key), "Key must exist");
    auto value =
        std::find_if(m_values->begin(), m_values->end(),
                     [&key](const auto& value) { return value.m_key == key; });
    ensures(value != m_values->end());
    return &(*value);
}

auto SettingsNode::at(const SettingsKeys& key) noexcept -> SettingsNode* {
    expectsMessage(contains(key), "Key must exist");
    SettingsNode* settings = this;
    for(const auto& keyPart : key) {
        settings = settings->at(keyPart);
    }
    return settings;
}

auto SettingsNode::at(const SettingsKeys& key) const noexcept
    -> const SettingsNode* {
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

auto operator<<(ostream& os, const SettingsNode& settings) noexcept
    -> ostream& {
    return stream(os, settings, "");
}
} // namespace execHelper::config
