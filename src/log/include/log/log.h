#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>
#include <ostream>
#include <string>

#include "consoleLogger.h"
#include "logLevel.h"
#include "logMessage.h"

namespace execHelper {
namespace log {
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

    /**
     * Set the minimum severity of each shown log message for a channel to the given
     * severity
     *
     * \param[in] channel   The channel to set the severity from
     * \param[in] severity  The severity to set the minimal severity to
     */
    void setSeverity(const Channel& channel, LogLevel severity);

  private:
    /**
     * Common init script initializing the logging system
     *
     * \param[in] logStream   The output stream to log to
     */
    void init(std::ostream& logStream) noexcept;

    std::unique_ptr<ConsoleLogger> m_consoleLogger;
};
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

auto operator<<(std::ostream& out, const Modifier& mod) -> std::ostream&;

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
    (std::cerr << execHelper::color::red << x << execHelper::color::def        \
               << std::endl); // NOLINT(misc-macro-parentheses)

#endif /* __LOG_H__ */
