#include "log.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/value_ref.hpp>

#include "assertions.h"

using std::make_unique;
using std::unique_ptr;

using boost::log::expressions::channel_severity_filter_actor;
using boost::log::expressions::smessage;
using boost::log::expressions::stream;
using boost::log::keywords::filter;
using boost::log::keywords::format;
using boost::log::sinks::synchronous_sink;
using boost::log::sinks::basic_text_ostream_backend;
using boost::shared_ptr;

using execHelper::log::none;
using execHelper::log::Channel;
using execHelper::log::LogLevel;

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)   // NOLINT(modernize-use-using)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", LogLevel)                     // NOLINT(modernize-use-using)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", Channel)                        // NOLINT(modernize-use-using)

namespace {
    class ConsoleLogger {
        public:
            explicit ConsoleLogger(std::ostream& logStream) :
                m_logMessageFilter(boost::log::expressions::channel_severity_filter(channel, severity)),
                m_consoleSink(add_console_log(logStream,
                                        filter = severity >= none && false,
                                        format = (stream << timestamp << " <" << severity << "> [" << channel << "] " << fileLog << ":" << lineLog << " " << smessage)))
            {
                ;
            }

            bool setSeverity(const Channel& channel, LogLevel severity) noexcept {
                m_logMessageFilter[channel] = severity;
                m_consoleSink->set_filter(m_logMessageFilter || false);
                return true;
            }

        private:
            channel_severity_filter_actor<Channel, LogLevel> m_logMessageFilter;
            shared_ptr<synchronous_sink<basic_text_ostream_backend<char>>> m_consoleSink;
    };

    unique_ptr<ConsoleLogger> consoleLogger;
} // namespace

namespace execHelper {
    namespace log {
        void init(std::ostream& logStream) noexcept {
            boost::log::add_common_attributes();
            boost::log::register_simple_formatter_factory< LogLevel, char >("Severity");

            consoleLogger = make_unique<ConsoleLogger>(logStream);
        }

        bool setSeverity(const Channel& channel, LogLevel severity) noexcept {
            return consoleLogger->setSeverity(channel, severity);
        }


    } // namespace log

    namespace color {
        std::ostream& operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    } // namespace color
} // namespace execHelper
