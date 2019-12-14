#ifndef __PATTERN_H__
#define __PATTERN_H__

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace execHelper::config {
using PatternKey = std::string;
using PatternValue = std::string;
using PatternKeys = std::vector<PatternKey>;
using PatternValues = std::vector<PatternValue>;
using ShortOption = std::optional<char>;
using LongOption = std::optional<std::string>;

using PatternCombinations = std::map<PatternKey, PatternValue>;

/**
 * \brief A pattern that can be used in expressions
 */
class Pattern {
  public:
    /**
     * Constructor
     *
     * \param[in] patternKey    The key of the pattern
     * \param[in] values        The initial values for the pattern
     * \param[in] shortOption   The short option associated with the pattern
     * \param[in] longOption    The long option associated with the pattern
     */
    explicit Pattern(PatternKey patternKey, PatternValues values = {},
                     ShortOption shortOption = std::nullopt,
                     LongOption longOption = std::nullopt) noexcept;

    /**
     * Equality operator
     *
     * \param[in] other The other object to compare with
     * \returns True    If the other object is considered equal
     *          False   Otherwise
     */
    auto operator==(const Pattern& other) const noexcept -> bool;

    /**
     * Inequality operator
     *
     * \param[in] other The other object to compare with
     * \returns ! \ref operator==(const Pattern& other) const
     *
     */
    auto operator!=(const Pattern& other) const noexcept -> bool;

    /**
     * Getter for the key
     *
     * \returns The key
     */
    [[nodiscard]] auto getKey() const noexcept -> const PatternKey&;

    /**
     * Getter for the values
     *
     * \returns The values
     */
    [[nodiscard]] auto getValues() const noexcept -> const PatternValues&;

    /**
     * Set the values for this pattern
     *
     * \param[in] values    The new values
     * \returns True    if the values were successfully set
     *          False   otherwise
     */
    [[nodiscard]] auto setValues(PatternValues values) noexcept -> bool;

    /**
     * Getter for the short option
     *
     * \returns The short option
     */
    [[nodiscard]] auto getShortOption() const noexcept -> const ShortOption&;

    /**
     * Getter for the long option
     *
     * \returns The long option
     */
    [[nodiscard]] auto getLongOption() const noexcept -> const LongOption&;

  private:
    PatternKey m_key;
    PatternValues m_values;
    ShortOption m_shortOption;
    LongOption m_longOption;
};
using Patterns = std::vector<Pattern>;

/**
 * Streaming operator for pattern objects
 *
 * \param os    The stream to stream to
 * \param pattern  The pattern to stream
 * \returns os
 */
auto operator<<(std::ostream& os, const Pattern& pattern) noexcept
    -> std::ostream&;
} // namespace execHelper::config

#endif /* __PATTERN__H__ */
