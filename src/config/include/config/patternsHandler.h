#ifndef __PATTERNS_HANDLER_H__
#define __PATTERNS_HANDLER_H__

#include <map>
#include <optional>

#include "config/variablesMap.h"

#include "pattern.h"

namespace execHelper::config {
/**
 * \brief Handles the patterns it is given
 */
class PatternsHandler {
  private:
    using PatternCollection = std::map<PatternKey, Pattern>;

  public:
    PatternsHandler() = default;

    /**
     * Construct a patterns handler with the given patterns
     *
     * \param[in] patterns  The patterns to add to the created patterns handler
     */
    explicit PatternsHandler(const Patterns& patterns);

    /**
     * Construct a patterns handler with the given patterns
     *
     * \param[in] patterns  The patterns to add to the created patterns handler
     */
    explicit PatternsHandler(Patterns&& patterns);

    /**
     * Equality operator
     *
     * \param[in] other The other object to compare with
     * \returns True    If the objects are considered equal
     *          False   Otherwise
     */
    auto operator==(const PatternsHandler& other) const noexcept -> bool;

    /**
     * Inequality operator
     *
     * \param[in] other The other object to compare with
     * \returns !operator==(other)
     */
    auto operator!=(const PatternsHandler& other) const noexcept -> bool;

    /**
     * Returns whether the pattern associated with the given key is registered
     *
     * \param[in] key   The key of the pattern
     * \returns True    If the handler handles the pattern
     *          False   Otherwise
     */
    [[nodiscard]] auto contains(const PatternKey& key) const noexcept -> bool;

    /**
     * Registers a pattern with the handler
     *
     * \param[in] pattern   The pattern to register
     */
    void addPattern(const Pattern& pattern) noexcept;

    /**
     * Returns the pattern associated with the given key
     *
     * \pre \ref contains(const PatternKey&) const for the given key returns true
     *
     * \param[in] key   The key of the pattern \returns The found pattern
     */
    [[nodiscard]] auto getPattern(const PatternKey& key) const noexcept
        -> const Pattern&;

    /**
     * Returns the default pattern map for a pattern
     *
     * \param[in] key   The root key to use for the newly created variables map
     * \returns The default variables map
     */
    [[nodiscard]] static auto
    getDefaultPatternMap(const PatternKey& key) noexcept
        -> config::VariablesMap;

    /**
     * Converts the given patternMap to a pattern
     *
     * \param[in] key   The key associated with the pattern
     * \param[in] patternMap    The variables map containing the values for the
     * pattern to create \returns The created pattern if the variables map is
     * valid boost::none otherwise
     */
    [[nodiscard]] static auto
    toPattern(const PatternKey& key,
              const config::VariablesMap& patternMap) noexcept
        -> std::optional<Pattern>;

  private:
    static auto convert(Patterns&& patterns) noexcept -> PatternCollection;

    PatternCollection m_patterns;
};
} // namespace execHelper::config

#endif /* __PATTERNS_HANDLER_H__ */
