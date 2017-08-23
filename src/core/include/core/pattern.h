#ifndef __PATTERN_H__
#define __PATTERN_H__

#include <string>
#include <vector>

namespace execHelper {
    namespace core {
        using PatternKey = std::string;
        using PatternValue = std::string;
        using PatternKeys = std::vector<PatternKey>;
        using PatternValues = std::vector<PatternValue>;

        class Pattern {
            public:
                Pattern(PatternKey patternKey, PatternValues defaultValues, char shortOption, std::string longOption) noexcept;

                bool operator==(const Pattern& other) const noexcept;
                bool operator!=(const Pattern& other) const noexcept;

                const PatternKey& getKey() const noexcept;
                const PatternValues& getDefaultValues() const noexcept;
                char getShortOption() const noexcept;
                const std::string& getLongOption() const noexcept;

            private:
                PatternKey m_key;
                PatternValues m_defaultValues;
                char m_shortOption;
                std::string m_longOption;
        };
    } // namespace core
} // namespace execHelper

#endif  /* __PATTERN__H__ */
