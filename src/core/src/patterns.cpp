#include "patterns.h"

#include <string>

#include <boost/algorithm/string/replace.hpp>

#include "logger.h"

using std::string;

namespace execHelper::core {
auto replacePatterns(const string& subject, const string& pattern,
                     const string& replacement) noexcept -> string {
    const auto needle = std::string("{").append(pattern).append("}");
    return boost::algorithm::replace_all_copy(subject, needle, replacement);
}
} // namespace execHelper::core
