#ifndef __PATTERN_H__
#define __PATTERN_H__

#include <string>
#include <vector>

namespace execHelper {
    namespace core {
        typedef std::string PatternKey;
        typedef std::string PatternValue;
        typedef std::vector<PatternKey> PatternKeys;
        typedef std::vector<PatternValue> PatternValues;

        class Pattern {
            public:
                Pattern(const PatternKey& patternKey, const PatternValues& defaultValue, char shortOption, const std::string& longOption) noexcept;

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
    }
}

#endif  /* __PATTERN__H__ */
