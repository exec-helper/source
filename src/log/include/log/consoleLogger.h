#ifndef CONSOLE_LOGGER_INCLUDE
#define CONSOLE_LOGGER_INCLUDE

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/shared_ptr.hpp>

#include "logLevel.h"
#include "logMessage.h"

namespace execHelper {
namespace log {
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

    /*! @copydoc config::Argv::Argv(const Argv&)
     */
    ConsoleLogger(const ConsoleLogger& other) = delete;

    /*! @copydoc config::Argv::Argv(Argv&&)
     */
    ConsoleLogger(ConsoleLogger&& other) = delete;

    ~ConsoleLogger();

    /*! @copydoc config::Argv::operator=(const Argv&)
     */
    ConsoleLogger& operator=(const ConsoleLogger& other) = delete;

    /*! @copydoc config::Argv::operator=(Argv&&)
     */
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
} // namespace log
} // namespace execHelper

#endif /* CONSOLE_LOGGER_INCLUDE */
