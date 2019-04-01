#include "patterns.h"

#include <regex>

#include "logger.h"

using std::regex;
using std::regex_error;
using std::regex_replace;
using std::string;

namespace execHelper::core {
string replacePatterns(const string& subject, const string& pattern,
                       const string& replacement) noexcept {
    const string PATTERN_PREFIX(R"(\{)");
    const string PATTERN_POSTFIX(R"(\})");
    try {
        return regex_replace(subject,
                             regex(PATTERN_PREFIX + pattern + PATTERN_POSTFIX),
                             replacement);
    } catch(regex_error&) {
        user_feedback("System warning: invalid regex for " << pattern);
        return subject;
    }
}
} // namespace execHelper::core
