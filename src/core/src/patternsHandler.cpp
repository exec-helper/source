#include "patternsHandler.h"

#include <assert.h>

using std::map;

using execHelper::core::Pattern;

namespace execHelper { namespace core {
    bool PatternsHandler::operator==(const PatternsHandler& other) const noexcept {
        return m_patterns == other.m_patterns;
    }

    bool PatternsHandler::operator!=(const PatternsHandler& other) const noexcept {
        return !(*this == other);
    }

    void PatternsHandler::addPattern(const Pattern& pattern) noexcept {
        m_patterns.emplace(pattern.getKey(), pattern);
    }

    bool PatternsHandler::contains(const PatternKey& key) const noexcept {
        return m_patterns.count(key) > 0U;
    }

    const Pattern& PatternsHandler::getPattern(const PatternKey& key) const noexcept {
        const PatternCollection& constPatterns = m_patterns;
        assert(constPatterns.count(key) > 0U);
        return constPatterns.at(key); 
    }
} }
