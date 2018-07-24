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

#include "logLevel.h"

BOOST_LOG_ATTRIBUTE_KEYWORD(fileLog, "File",
                            std::string) // NOLINT(modernize-use-using)
BOOST_LOG_ATTRIBUTE_KEYWORD(lineLog, "Line",
                            unsigned int) // NOLINT(modernize-use-using)

namespace execHelper {
namespace log {
using Channel = std::string;
using LoggerType =
    boost::log::sources::severity_channel_logger_mt<LogLevel, std::string>;

/**
 * \brief Class for initializing and destructing the logging system. Must be created and kept alive while the logging system is being used.
 *
 * RAII object for initalizing and deinitializing the logging system
 */
class LogInit {
  public:
    /**
     * Initialize logging
     */
    LogInit() noexcept;

    /**
     * Initialize logging
     *
     * \param[in] logStream   The output stream to log to
     */
    explicit LogInit(std::ostream& logStream) noexcept;

    ~LogInit();

  private:
    /**
     * Common init script initializing the logging system
     *
     * \param[in] logStream   The output stream to log to
     */
    void init(std::ostream& logStream) noexcept;
};

/**
 * Set the minimum severity of each shown log message for a channel to the given
 * severity
 *
 * \param[in] channel   The channel to set the severity from
 * \param[in] severity  The severity to set the minimal severity to
 * \returns True    If the severity was successfully set
 *          False   Otherwise
 */
bool setSeverity(const Channel& channel, LogLevel severity) noexcept;
} // namespace log

namespace color {
enum ColorCode {
    FG_RED = 31,
    FG_GREEN = 32,
    FG_BLUE = 34,
    FG_DEFAULT = 39,
    BG_RED = 41,
    BG_GREEN = 42,
    BG_BLUE = 44,
    BG_DEFAULT = 49
};

/**
 * \brief Modifies the command line output format
 */
struct Modifier {
    ColorCode code; //!< The color code to use for the modifier

    /**
     * The modifier to use
     *
     * \param[in] cCode The color code to use
     */
    explicit Modifier(ColorCode cCode) noexcept : code(cCode) {}
};

std::ostream& operator<<(std::ostream& os, const Modifier& mod);

static Modifier def(FG_DEFAULT);
static Modifier blue(FG_BLUE);
static Modifier red(FG_RED);
} // namespace color
} // namespace execHelper

#define user_feedback(x)                                                       \
    (std::cout << x << std::endl) // NOLINT(misc-macro-parentheses)
#define user_feedback_info(x)                                                  \
    (std::cout << execHelper::color::blue << x << execHelper::color::def       \
               << std::endl); // NOLINT(misc-macro-parentheses)
#define user_feedback_error(x)                                                 \
    (std::cout << execHelper::color::red << x << execHelper::color::def        \
               << std::endl); // NOLINT(misc-macro-parentheses)

#endif /* __LOG_H__ */
