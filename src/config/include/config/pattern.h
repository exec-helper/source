#ifndef __PATTERN_H__
#define __PATTERN_H__

#include <map>
#include <string>
#include <vector>

#include <boost/optional/optional.hpp>

namespace execHelper {
namespace config {
using PatternKey = std::string;
using PatternValue = std::string;
using PatternKeys = std::vector<PatternKey>;
using PatternValues = std::vector<PatternValue>;
using ShortOption = boost::optional<char>;
using LongOption = boost::optional<std::string>;

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
    Pattern(PatternKey patternKey, PatternValues values = {},
            ShortOption shortOption = boost::none,
            LongOption longOption = boost::none) noexcept;

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
     * Getter for the values
     *
     * \returns The values
     */
    const PatternValues& getValues() const noexcept;

    /**
     * Set the values for this pattern
     *
     * \param[in] values    The new values
     * \returns True    if the values were successfully set
     *          False   otherwise
     */
    bool setValues(PatternValues values) noexcept;

    /**
     * Getter for the short option
     *
     * \returns The short option
     */
    const ShortOption& getShortOption() const noexcept;

    /**
     * Getter for the long option
     *
     * \returns The long option
     */
    const LongOption& getLongOption() const noexcept;

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
std::ostream& operator<<(std::ostream& os, const Pattern& pattern) noexcept;

} // namespace config
} // namespace execHelper

#endif /* __PATTERN__H__ */
