#ifndef __PATTERNS_H__
#define __PATTERNS_H__

#include <string>
#include <vector>

namespace execHelper {
    namespace core {
        class CompilerDescriptionElement;
    }    
}

namespace execHelper {
    namespace core {
        typedef std::vector<std::string> Patterns;

        std::string replacePatterns(const std::string& subject, const std::string& pattern, const std::string& replacement) noexcept;
        std::string replacePatterns(const std::string& subject, const Patterns& patterns, const core::CompilerDescriptionElement& compiler) noexcept;
    }
}

#endif  /* __PATTERNS_H__ */
