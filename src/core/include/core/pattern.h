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

        /**
         * \brief A pattern that can be used in expressions
         */
        class Pattern {
            public:
                /**
                 * Constructor
                 *
                 * \param[in] patternKey    The key of the pattern
                 * \param[in] defaultValues The default value for the pattern
                 * \param[in] shortOption   The short option associated with the pattern
                 * \param[in] longOption    The long option associated with the pattern
                 */
                Pattern(PatternKey patternKey, PatternValues defaultValues, char shortOption, std::string longOption) noexcept;

                /**
                 * Equality operator
                 *
                 * \param[in] other The other object to compare with
                 * \returns True    If the other object is considered equal
                 *          False   Otherwise
                 */
                bool operator==(const Pattern& other) const noexcept;

                /**
                 * Inequality operator
                 *
                 * \param[in] other The other object to compare with
                 * \returns ! \ref operator==(const Pattern& other) const
                 *
                 */
                bool operator!=(const Pattern& other) const noexcept;

                /**
                 * Getter for the key
                 *
                 * \returns The key
                 */
                const PatternKey& getKey() const noexcept;

                /**
                 * Getter for the default pattern value
                 *
                 * \returns The default values
                 */
                const PatternValues& getDefaultValues() const noexcept;

                /**
                 * Getter for the short option
                 *
                 * \returns The short option
                 */
                char getShortOption() const noexcept;

                /**
                 * Getter for the long option
                 *
                 * \returns The long option
                 */
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
