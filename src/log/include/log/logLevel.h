#ifndef LOG_LEVEL_INCLUDE
#define LOG_LEVEL_INCLUDE

#include <string_view>
#include <vector>

namespace execHelper {
namespace log {
/**
 * \brief   List of the potential log levels
 */
enum LogLevel {
    all,  // Use this to enable all logging
    test, // Use this for printing (debug) output in tests
    trace,
    debug,
    info,
    warning,
    error,
    fatal,
    none // Use this to disable logging
};

/**
 * Convert the given log level string to a log level
 *
 * \param[in] level The log level string to convert
 * \returns The associated log level
 * \throws  InvalidLogLevel If the given level string is not associated with any
 * log level
 */
LogLevel toLogLevel(std::string_view level);

/**
 * Get all available log levels. This is convenient for looping over all values in the enumeration.
 *
 * \returns A collection of all log levels, sorted from most restrictive to least restrictive
 */
const std::vector<LogLevel>& getLogLevels();

/**
 * Convert the log level to a string
 *
 * \param[in] level The level to convert
 * \returns The string associated with the log level
 */
std::string_view toString(LogLevel level) noexcept;

auto operator<<(std::ostream& out, LogLevel level) noexcept -> std::ostream&;

/**
 * \brief   Thrown if an unknown log level is used
 */
struct InvalidLogLevel : public std::exception {};
} // namespace log
} // namespace execHelper

#endif /* LOG_LEVEL_INCLUDE */
