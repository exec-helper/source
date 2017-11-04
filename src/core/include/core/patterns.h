#ifndef __PATTERNS_H__
#define __PATTERNS_H__

#include <string>

#include "config/pattern.h"

#include "mapPermutator.h"

namespace execHelper {
namespace core {
/**
 * Replaces all occurences of the given pattern in the subject with replacement
 *
 * \param[in] subject   The subject
 * \param[in] pattern   The pattern
 * \param[in] replacement   The replacement
 * \returns The replaced string
 */
std::string replacePatterns(const std::string& subject,
                            const std::string& pattern,
                            const std::string& replacement) noexcept;
} // namespace core
} // namespace execHelper

#endif /* __PATTERNS_H__ */
