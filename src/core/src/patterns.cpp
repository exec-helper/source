#include "patterns.h"

#include <regex>

#include "log/log.h"

using std::string;
using std::regex;
using std::regex_replace;

namespace {
    const string PATTERN_PREFIX(R"(\{)");
    const string PATTERN_POSTFIX(R"(\})");
}

namespace execHelper { namespace core {
    string replacePatterns(const string& subject, const string& pattern, const string& replacement) noexcept {
        return regex_replace(subject, regex(PATTERN_PREFIX + pattern + PATTERN_POSTFIX), replacement);
    }
} }
