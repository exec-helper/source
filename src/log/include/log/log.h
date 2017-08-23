#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>
#include <ostream>
#include <string>

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/expressions.hpp>
#include <boost/log/sources/channel_feature.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

BOOST_LOG_ATTRIBUTE_KEYWORD(fileLog, "File", std::string)   // NOLINT(modernize-use-using)
BOOST_LOG_ATTRIBUTE_KEYWORD(lineLog, "Line", unsigned int)  // NOLINT(modernize-use-using)

namespace execHelper {
    namespace log {
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

        using Channel = std::string;
        using LoggerType = boost::log::sources::severity_channel_logger_mt<LogLevel, std::string>;

        void init(std::ostream& logStream = std::clog) noexcept;
        bool setSeverity(const Channel& channel, LogLevel severity) noexcept;

        // Throws InvalidLogLevel
        LogLevel toLogLevel(const std::string& level);
        std::string toString(LogLevel level) noexcept;

        std::ostream& operator<<(std::ostream& os, LogLevel level);

        struct InvalidLogLevel : public std::exception {};
    } // namespace log

    namespace color {
        enum ColorCode {
            FG_RED      = 31,
            FG_GREEN    = 32,
            FG_BLUE     = 34,
            FG_DEFAULT  = 39,
            BG_RED      = 41,
            BG_GREEN    = 42,
            BG_BLUE     = 44,
            BG_DEFAULT  = 49
        };

        /**
         * \brief Modifies the command line output format
         */
        class Modifier {
            ColorCode code;
        public:
            explicit Modifier(ColorCode cCode) noexcept : code(cCode) {}
            friend std::ostream&
            operator<<(std::ostream& os, const Modifier& mod) {
                return os << "\033[" << mod.code << "m";
            }

        };

        static Modifier def(FG_DEFAULT);
        static Modifier blue(FG_BLUE);
        static Modifier red(FG_RED);
    } // namespace color
} // namespace execHelper

#define user_feedback(x) (std::cout << x << std::endl)      // NOLINT(misc-macro-parentheses)
#define user_feedback_info(x) (std::cout << execHelper::color::blue << x << execHelper::color::def << std::endl);   // NOLINT(misc-macro-parentheses)
#define user_feedback_error(x) (std::cout << execHelper::color::red << x << execHelper::color::def << std::endl);   // NOLINT(misc-macro-parentheses)

#endif  /* __LOG_H__ */
