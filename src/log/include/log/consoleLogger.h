#ifndef CONSOLE_LOGGER_INCLUDE
#define CONSOLE_LOGGER_INCLUDE

#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/shared_ptr.hpp>

#include "logLevel.h"
#include "logMessage.h"

namespace execHelper::log {
/**
 * \brief Frontend wrapper for the logger that logs to the console
 */
class ConsoleLogger {
  public:
    /**
     * Create a new console logger
     *
     * \param[in] logStream The console stream to log to
     */
    explicit ConsoleLogger(std::ostream& logStream);

    ConsoleLogger(const ConsoleLogger& other) = delete;
    ConsoleLogger(ConsoleLogger&& other) = delete;

    ~ConsoleLogger();

    ConsoleLogger& operator=(const ConsoleLogger& other) = delete;
    ConsoleLogger& operator=(ConsoleLogger&& other) = delete;

    /**
     * Set the maximum severity of the messages to a channel to log
     *
     * \param[in] channel   The channel for which to set the severity
     * \param[in] severity  The maximum severity to set for the specified channel
     * \returns True    If the severity was correctly set
     *          False   Otherwise
     */
    bool setSeverity(const Channel& channel, LogLevel severity) noexcept;

  private:
    boost::log::expressions::channel_severity_filter_actor<Channel, LogLevel>
        m_logMessageFilter; //!< Sets the filters for channels and severities
    boost::shared_ptr<boost::log::sinks::synchronous_sink<
        boost::log::sinks::basic_text_ostream_backend<char>>>
        m_consoleSink; //!< The backend for this logger
};
} // namespace execHelper::log

#endif /* CONSOLE_LOGGER_INCLUDE */
