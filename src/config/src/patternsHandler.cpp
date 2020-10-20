#include "patternsHandler.h"

#include <algorithm>
#include <optional>
#include <string>

#include <log/assertions.h>

#include "logger.h"
#include "variablesMap.h"

using std::inserter;
using std::move;
using std::optional;
using std::pair;
using std::string;
using std::transform;

namespace execHelper::config {
PatternsHandler::PatternsHandler(const Patterns& patterns)
    : m_patterns(convert(Patterns(patterns))) {
    ;
}

PatternsHandler::PatternsHandler(Patterns&& patterns)
    : m_patterns(convert(move(patterns))) {
    ;
}

auto PatternsHandler::operator==(const PatternsHandler& other) const noexcept
    -> bool {
    return m_patterns == other.m_patterns;
}

auto PatternsHandler::operator!=(const PatternsHandler& other) const noexcept
    -> bool {
    return !(*this == other);
}

void PatternsHandler::addPattern(const Pattern& pattern) noexcept {
    m_patterns.emplace(pattern.getKey(), pattern);
}

auto PatternsHandler::contains(const PatternKey& key) const noexcept -> bool {
    return m_patterns.count(key) > 0U;
}

auto PatternsHandler::getPattern(const PatternKey& key) const noexcept
    -> const Pattern& {
    const PatternCollection& constPatterns = m_patterns;
    ensures(constPatterns.count(key) > 0U);
    return constPatterns.at(key);
}

auto PatternsHandler::getDefaultPatternMap(const PatternKey& key) noexcept
    -> config::VariablesMap {
    return VariablesMap(key);
}

auto PatternsHandler::toPattern(const PatternKey& key,
                                const VariablesMap& patternMap) noexcept
    -> optional<Pattern> {
    static const string DEFAULT_VALUES_KEY("default-values");
    static const string SHORT_OPTION_KEY("short-option");
    static const string LONG_OPTION_KEY("long-option");

    auto defaultValues = patternMap.get<PatternValues>(DEFAULT_VALUES_KEY);
    if(defaultValues == std::nullopt || defaultValues.value().empty()) {
        user_feedback_error(
            "The default values must be defined and can not be empty");
        return std::nullopt;
    }

    auto shortOption = patternMap.get<char>(SHORT_OPTION_KEY);
    auto longOptionOpt = patternMap.get<string>(LONG_OPTION_KEY);
    return Pattern(key, *defaultValues, shortOption, longOptionOpt);
}

inline auto PatternsHandler::convert(Patterns&& patterns) noexcept
    -> PatternCollection {
    execHelper::config::PatternsHandler::PatternCollection result;
    transform(
        patterns.begin(), patterns.end(), inserter(result, result.end()),
        [](auto&& pattern) -> pair<execHelper::config::PatternKey,
                                   execHelper::config::Pattern> {
            return {pattern.getKey(), std::forward<decltype(pattern)>(pattern)};
        });
    return result;
}
} // namespace execHelper::config
