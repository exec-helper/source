#include "patterns.h"

#include <boost/algorithm/string/replace.hpp>

#include "logger.h"

using boost::replace_all;
using std::string;

namespace execHelper {
namespace core {
string replacePatterns(const string& subject, const string& pattern,
                       const string& replacement) noexcept {
    	const string PATTERN_PREFIX("{");
    	const string PATTERN_POSTFIX("}");
        auto result = subject;
	replace_all(result, PATTERN_PREFIX + pattern + PATTERN_POSTFIX, replacement);
	return result;
}
} // namespace core
} // namespace execHelper
