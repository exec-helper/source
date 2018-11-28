#include "pattern.h"

#include <ostream>
#include <regex>
#include <utility>

using std::move;
using std::ostream;
using std::regex;
using std::string;

namespace execHelper {
namespace config {
// cppcheck-suppress passedByValue symbolName=patternKey
// cppcheck-suppress passedByValue symbolName=values
Pattern::Pattern(PatternKey patternKey, PatternValues values,
                 ShortOption shortOption, LongOption longOption) noexcept
    : m_key(move(patternKey)),
      m_values(move(values)),
      m_shortOption(shortOption),
      m_longOption(std::move(longOption)) {
    ;
}

bool Pattern::operator==(const Pattern& other) const noexcept {
    return (m_key == other.m_key && m_values == other.m_values &&
            m_shortOption == other.m_shortOption &&
            m_longOption == other.m_longOption);
}

bool Pattern::operator!=(const Pattern& other) const noexcept {
    return !(*this == other);
}

const PatternKey& Pattern::getKey() const noexcept { return m_key; }

const PatternValues& Pattern::getValues() const noexcept { return m_values; }

bool Pattern::setValues(PatternValues values) noexcept {
    m_values = std::move(values);
    return true;
}

const ShortOption& Pattern::getShortOption() const noexcept {
    return m_shortOption;
}

const LongOption& Pattern::getLongOption() const noexcept {
    return m_longOption;
}

ostream& operator<<(ostream& os, const Pattern& pattern) noexcept {
    os << "{" << pattern.getKey() << ": ";
    auto shortOption = pattern.getShortOption();
    if(shortOption) {
        os << "short option: " << shortOption.value() << ", ";
    }
    auto longOption = pattern.getLongOption();
    if(longOption) {
        os << "long option: " << longOption.value() << ", ";
    }
    os << "values: {";
    for(const auto& value : pattern.getValues()) {
        os << value << ";";
    }
    os << "}";
    os << "}";
    return os;
}
} // namespace config
} // namespace execHelper
