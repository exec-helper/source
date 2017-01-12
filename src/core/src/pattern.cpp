#include "pattern.h"

#include <regex>

#include "options.h"

using std::string;
using std::regex;
using std::regex_replace;

namespace {
    const string PATTERN_PREFIX("\\{");
    const string PATTERN_POSTFIX("\\}");
}

namespace execHelper { namespace core {
    Pattern::Pattern(const PatternKey& patternKey, const PatternValues& defaultValues, char shortOption, const std::string& longOption) noexcept :
        m_key(patternKey),
        m_defaultValues(defaultValues),
        m_shortOption(shortOption),
        m_longOption(longOption)
    {
        ;
    }

    bool Pattern::operator==(const Pattern& other) const noexcept {
        return (m_key == other.m_key && m_defaultValues == other.m_defaultValues && m_shortOption == other.m_shortOption && m_longOption == other.m_longOption);
    }

    bool Pattern::operator!=(const Pattern& other) const noexcept {
        return !(*this == other);
    }

    const PatternKey& Pattern::getKey() const noexcept {
        return m_key;
    }

    const PatternValues& Pattern::getDefaultValues() const noexcept {
        return m_defaultValues;
    }

    char Pattern::getShortOption() const noexcept {
        return m_shortOption;
    }

    const string& Pattern::getLongOption() const noexcept {
        return m_longOption;
    }
} }
