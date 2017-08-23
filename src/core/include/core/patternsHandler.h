#ifndef __PATTERNS_HANDLER_H__
#define __PATTERNS_HANDLER_H__

#include <map>

#include "pattern.h"

namespace execHelper {
    namespace core {
        class PatternsHandler {
            private:
                using PatternCollection = std::map<PatternKey, Pattern>;

            public:
                bool operator==(const PatternsHandler& other) const noexcept;
                bool operator!=(const PatternsHandler& other) const noexcept;

                bool contains(const PatternKey& key) const noexcept;
                void addPattern(const Pattern& pattern) noexcept;
                const Pattern& getPattern(const PatternKey& key) const noexcept;

            private:
                PatternCollection m_patterns;
        };
    } // namespace core
} // namespace execHelper

#endif  /* __PATTERNS_HANDLER_H__ */
