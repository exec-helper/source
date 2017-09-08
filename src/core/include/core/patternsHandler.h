#ifndef __PATTERNS_HANDLER_H__
#define __PATTERNS_HANDLER_H__

#include <map>

#include "pattern.h"

namespace execHelper {
    namespace core {
        /**
         * \brief Handles the patterns it is given
         */
        class PatternsHandler {
            private:
                using PatternCollection = std::map<PatternKey, Pattern>;

            public:
                /**
                 * Equality operator
                 *
                 * \param[in] other The other object to compare with
                 * \returns True    If the other object is considered equal
                 *          False   Otherwise
                 */
                bool operator==(const PatternsHandler& other) const noexcept;

                /**
                 * Inequality operator
                 *
                 * \param[in] other The other object to compare with
                 * \returns ! \ref operator==(const PatternsHandler& other) const
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
                 * \pre \ref contains(const PatternKey&) const for the given key returns true
                 * \param[in] key   The key of the pattern
                 * \returns The found pattern
                 */
                const Pattern& getPattern(const PatternKey& key) const noexcept;

            private:
                PatternCollection m_patterns;
        };
    } // namespace core
} // namespace execHelper

#endif  /* __PATTERNS_HANDLER_H__ */
