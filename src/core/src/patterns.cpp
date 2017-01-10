#include "patterns.h"

#include <regex>

using std::string;
using std::regex;
using std::regex_replace;

namespace {
    const string PATTERN_PREFIX("\\{");
    const string PATTERN_POSTFIX("\\}");
}

namespace execHelper { namespace core {
    string replacePatterns(const string& subject, const string& pattern, const string& replacement) noexcept {
        return regex_replace(subject, regex(PATTERN_PREFIX + pattern + PATTERN_POSTFIX), replacement);
    }
} }
