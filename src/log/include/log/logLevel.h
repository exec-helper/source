#ifndef LOG_LEVEL_INCLUDE
#define LOG_LEVEL_INCLUDE

#include <string>

namespace execHelper {
    namespace log {
         /**
         * \brief   List of the potential log levels
         */
        enum LogLevel
        {
            all,        // Use this to enable all logging
            trace,
            debug,
            info,
            warning,
            error,
            fatal,
            none        // Use this to disable logging
        };

        /**
         * Convert the given log level string to a log level
         *
         * \param[in] level The log level string to convert
         * \returns The associated log level
         * \throws  InvalidLogLevel If the given level string is not associated with any log level
         */
        LogLevel toLogLevel(const std::string& level);

        /**
         * Convert the log level to a string
         *
         * \param[in] level The level to convert
         * \returns The string associated with the log level
         */
        std::string toString(LogLevel level) noexcept;

        std::ostream& operator<<(std::ostream& os, LogLevel level);

        /**
         * \brief   Thrown if an unknown log level is used
         */
        struct InvalidLogLevel : public std::exception {};
    } // namespace log
} // namespace execHelper

#endif  /* LOG_LEVEL_INCLUDE */
