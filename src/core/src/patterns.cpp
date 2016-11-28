#include "patterns.h"

#include <regex>

#include "execHelperOptions.h"

using std::string;
using std::regex;
using std::regex_replace;
using execHelper::core::ExecHelperOptions;

namespace {
    const string PATTERN_PREFIX("\\{");
    const string PATTERN_POSTFIX("\\}");
}

namespace execHelper { namespace core {
    string replacePatterns(const string& subject, const string& pattern, const string& replacement) noexcept {
        return regex_replace(subject, regex(PATTERN_PREFIX + pattern + PATTERN_POSTFIX), replacement);
    }

    string replacePatterns(const string& subject, const Patterns& patterns, const CompilerDescription& compiler) noexcept {
        string result(subject);
        for(const auto& pattern : patterns) {
            if(pattern == "COMPILER") {
                result = replacePatterns(result, pattern, compiler.getCompilers()[0].getName());  
            } else if(pattern == "MODE") {
                result = replacePatterns(result, pattern, compiler.getModes()[0].getMode());  
            }
        }
        return result;
    }
} }
