#include "logLevel.h"

#include <algorithm>
#include <vector>

#include "log/assertions.h"

using std::distance;
using std::find;
using std::string;
using std::vector;

namespace {
inline const vector<string>& getLogLevelStrings() noexcept {
    static const vector<string> logLevels({"all", "test", "trace", "debug",
                                           "info", "warning", "error", "fatal",
                                           "none"});
    return logLevels;
}
} // namespace

namespace execHelper {
namespace log {
LogLevel toLogLevel(const std::string& level) {
    const auto& logLevelStrings = getLogLevelStrings();
    const auto& element =
        find(logLevelStrings.begin(), logLevelStrings.end(), level);
    if(element == logLevelStrings.end()) {
        throw InvalidLogLevel();
    }
    auto index = distance(logLevelStrings.begin(), element);
    return static_cast<LogLevel>(index);
}

std::ostream& operator<<(std::ostream& os, LogLevel level) {
    os << toString(level);
    return os;
}

string toString(LogLevel level) noexcept {
    expectsMessage(level < getLogLevelStrings().size(),
                   "Level must be a log level value");
    return getLogLevelStrings()[level];
}
} // namespace log
} // namespace execHelper
