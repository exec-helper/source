#include "consoleLogger.h"

#include <boost/log/keywords/filter.hpp>
#include <boost/log/utility/setup/console.hpp>

using boost::log::expressions::smessage;
using boost::log::expressions::stream;
using boost::log::keywords::filter;
using boost::log::keywords::format;

namespace execHelper::log {
ConsoleLogger::ConsoleLogger(std::ostream& logStream)
    : m_logMessageFilter(
          boost::log::expressions::channel_severity_filter(channel, severity)),
      m_consoleSink(add_console_log(
          logStream, filter = severity >= none && false,
          format = (stream << timestamp << " <" << severity << "> [" << channel
                           << "] " << fileLog << ":" << lineLog << " "
                           << smessage))) {}

ConsoleLogger::~ConsoleLogger() {
    if(m_consoleSink) {
        boost::log::core::get()->remove_sink(m_consoleSink);
        m_consoleSink->flush();
        m_consoleSink.reset();
    }
}

bool ConsoleLogger::setSeverity(const Channel& channel,
                                LogLevel severity) noexcept {
    m_logMessageFilter[channel] = severity;
    m_consoleSink->set_filter(m_logMessageFilter || false);
    return true;
}
} // namespace execHelper::log
