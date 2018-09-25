#ifndef __PATTERNS_HANDLER_H__
#define __PATTERNS_HANDLER_H__

#include <map>
#include <optional>

#include "config/variablesMap.h"

#include "pattern.h"

namespace execHelper {
namespace config {
/**
 * \brief Handles the patterns it is given
 */
class PatternsHandler {
  private:
    using PatternCollection = std::map<PatternKey, Pattern>;

  public:
    PatternsHandler() = default;

    /*! @copydoc config::Argv::Argv(const Argv& patterns)
     */
    explicit PatternsHandler(const Patterns& other);

    /*! @copydoc config::Argv::Argv(Argv&& patterns)
     */
    explicit PatternsHandler(Patterns&& other) noexcept;

    /*! @copydoc config::Argv::operator==(const config::Argv& other) const
     */
    bool operator==(const PatternsHandler& other) const noexcept;

    /*! @copydoc config::Argv::operator!=(const config::Argv& other) const
     */
    bool operator!=(const PatternsHandler& other) const noexcept;

    /**
     * Returns whether the pattern associated with the given key is registered
     *
     * \param[in] key   The key of the pattern
     * \returns True    If the handler handles the pattern
     *          False   Otherwise
     */
    bool contains(const PatternKey& key) const noexcept;

    /**
     * Registers a pattern with the handler
     *
     * \param[in] pattern   The pattern to register
     */
    void addPattern(const Pattern& pattern) noexcept;

    /**
     * Returns the pattern associated with the given key
     *
     * \pre \ref contains(const PatternKey&) const for the given key returns
     * true \param[in] key   The key of the pattern \returns The found pattern
     */
    const Pattern& getPattern(const PatternKey& key) const noexcept;

    /**
     * Returns the default pattern map for a pattern
     *
     * \param[in] key   The root key to use for the newly created variables map
     * \returns The default variables map
     */
    static config::VariablesMap
    getDefaultPatternMap(const PatternKey& key) noexcept;

    /**
     * Converts the given patternMap to a pattern
     *
     * \param[in] key   The key associated with the pattern
     * \param[in] patternMap    The variables map containing the values for the
     * pattern to create \returns The created pattern if the variables map is
     * valid boost::none otherwise
     */
    static std::optional<Pattern>
    toPattern(const PatternKey& key,
              const config::VariablesMap& patternMap) noexcept;

  private:
    PatternCollection m_patterns;
};
} // namespace config
} // namespace execHelper

#endif /* __PATTERNS_HANDLER_H__ */
