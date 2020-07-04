#include "logLevel.h"

#include <algorithm>
#include <sstream>
#include <vector>

#include "log/assertions.h"

using std::distance;
using std::find;
using std::string_view;
using std::vector;

namespace {
inline auto getLogLevelStrings() noexcept -> const vector<string_view>& {
    using namespace std::literals;
    static const vector<string_view> logLevels(
        {"all"sv, "test"sv, "trace"sv, "debug"sv, "info"sv, "warning"sv,
         "error"sv, "fatal"sv, "none"sv});
    return logLevels;
}
} // namespace

namespace execHelper::log {
auto toLogLevel(std::string_view level) -> LogLevel {
    const auto& logLevelStrings = getLogLevelStrings();
    const auto& element =
        find(logLevelStrings.begin(), logLevelStrings.end(), level);
    if(element == logLevelStrings.end()) {
        throw InvalidLogLevel();
    }
    auto index = distance(logLevelStrings.begin(), element);
    return static_cast<LogLevel>(index);
}

auto getLogLevels() -> const vector<LogLevel>& {
    static const vector<LogLevel> LOG_LEVELS(
        {none, fatal, error, warning, info, debug, trace, all});
    return LOG_LEVELS;
}

auto operator<<(std::ostream& os, LogLevel level) noexcept -> std::ostream& {
    os << toString(level);
    return os;
}

auto toString(LogLevel level) noexcept -> string_view {
    expectsMessage(level < getLogLevelStrings().size(),
                   "Level must be a log level value");
    return getLogLevelStrings()[level];
}
} // namespace execHelper::log
