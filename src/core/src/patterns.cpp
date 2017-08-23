#include "patterns.h"

#include <regex>

using std::string;
using std::regex;
using std::regex_replace;

namespace execHelper { namespace core {
    string replacePatterns(const string& subject, const string& pattern, const string& replacement) noexcept {
        const string PATTERN_PREFIX(R"(\{)");
        const string PATTERN_POSTFIX(R"(\})");
        return regex_replace(subject, regex(PATTERN_PREFIX + pattern + PATTERN_POSTFIX), replacement);
    }
} // namespace core
} // namespace execHelper
