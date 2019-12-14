#include "pattern.h"

#include <ostream>
#include <utility>

using std::move;
using std::ostream;
using std::string;

namespace execHelper::config {
Pattern::Pattern(PatternKey patternKey, PatternValues values,
                 ShortOption shortOption, LongOption longOption) noexcept
    : m_key(move(patternKey)),
      m_values(move(values)),
      m_shortOption(shortOption),
      m_longOption(std::move(longOption)) {
    ;
}

auto Pattern::operator==(const Pattern& other) const noexcept -> bool {
    return (m_key == other.m_key && m_values == other.m_values &&
            m_shortOption == other.m_shortOption &&
            m_longOption == other.m_longOption);
}

auto Pattern::operator!=(const Pattern& other) const noexcept -> bool {
    return !(*this == other);
}

auto Pattern::getKey() const noexcept -> const PatternKey& { return m_key; }

auto Pattern::getValues() const noexcept -> const PatternValues& {
    return m_values;
}

auto Pattern::setValues(PatternValues values) noexcept -> bool {
    m_values = std::move(values);
    return true;
}

auto Pattern::getShortOption() const noexcept -> const ShortOption& {
    return m_shortOption;
}

auto Pattern::getLongOption() const noexcept -> const LongOption& {
    return m_longOption;
}

auto operator<<(ostream& os, const Pattern& pattern) noexcept -> ostream& {
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
} // namespace execHelper::config
