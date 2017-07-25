#include "patternsHandler.h"

#include <log/assertions.h>
#include <string>

#include "logger.h"

using std::string;
using std::vector;

using boost::optional;

namespace execHelper { namespace config {
    PatternsHandler::PatternsHandler(const Patterns& other) {
        for(auto pattern : other) {
            m_patterns.emplace(pattern.getKey(), pattern);
        }
    }

    PatternsHandler::PatternsHandler(Patterns&& other) noexcept {
        for(auto&& pattern : other) {
            m_patterns.emplace(pattern.getKey(), pattern);
        }
    }

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
        ensures(constPatterns.count(key) > 0U);
        return constPatterns.at(key); 
    }

    config::VariablesMap PatternsHandler::getDefaultPatternMap(const PatternKey& key) noexcept {
        return VariablesMap(key);
    }

    optional<Pattern> PatternsHandler::toPattern(const PatternKey& key, const VariablesMap& patternMap) noexcept {
        static const string DEFAULT_VALUES_KEY("default-values");
        static const string SHORT_OPTION_KEY("short-option");
        static const string LONG_OPTION_KEY("long-option");

        auto defaultValues = patternMap.get<PatternValues>(DEFAULT_VALUES_KEY);
        if(defaultValues == boost::none || defaultValues.get().empty()) {
            user_feedback_error("The default values must be defined and can not be empty");
            return boost::none;
        }

        auto shortOption = patternMap.get<char>(SHORT_OPTION_KEY);
        auto longOptionOpt = patternMap.get<string>(LONG_OPTION_KEY);
        return Pattern(key, defaultValues.get(), shortOption, longOptionOpt);
    }
} // namespace config
} // namespace execHelper
