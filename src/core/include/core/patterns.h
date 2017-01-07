#ifndef __PATTERNS_H__
#define __PATTERNS_H__

#include <string>
#include <vector>

#include "options.h"

namespace execHelper {
    namespace core {
        class TargetDescriptionElement;
        class CompilerDescriptionElement;
    }    
}

namespace execHelper {
    namespace core {
        std::string replacePatterns(const std::string& subject, const std::string& pattern, const std::string& replacement) noexcept;
        std::string replacePatterns(const std::string& subject, const PatternKeys& patterns, const core::TargetDescriptionElement& target) noexcept;
        std::string replacePatterns(const std::string& subject, const PatternKeys& patterns, const core::CompilerDescriptionElement& compiler) noexcept;
    }
}

#endif  /* __PATTERNS_H__ */
