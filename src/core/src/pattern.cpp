#include "pattern.h"

#include <regex>
#include <utility>

#include "options.h"

using std::move;
using std::string;
using std::regex;

namespace execHelper { namespace core {
    Pattern::Pattern(PatternKey patternKey, PatternValues defaultValues, char shortOption, std::string longOption) noexcept :
        m_key(move(patternKey)),
        m_defaultValues(move(defaultValues)),
        m_shortOption(shortOption),
        m_longOption(move(longOption))
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
} // namespace core
} // namespace execHelper
