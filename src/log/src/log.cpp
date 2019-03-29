#include "log.h"

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

#include "assertions.h"

using std::make_unique;

namespace execHelper {
namespace log {
LogInit::LogInit() noexcept { init(std::clog); }

LogInit::LogInit(std::ostream& logStream) noexcept { init(logStream); }

LogInit::~LogInit() { m_consoleLogger.reset(); }

void LogInit::init(std::ostream& logStream) noexcept {
    boost::log::add_common_attributes();
    boost::log::register_simple_formatter_factory<LogLevel, char>("Severity");

    m_consoleLogger = make_unique<ConsoleLogger>(logStream);
}

bool LogInit::setSeverity(const Channel& channel, LogLevel severity) noexcept {
    return m_consoleLogger->setSeverity(channel, severity);
}

} // namespace log

namespace color {
std::ostream& operator<<(std::ostream& os, const Modifier& mod) {
    return os << "\033[" << mod.code << "m";
}

std::wostream& operator<<(std::wostream& os, const Modifier& mod) {
    return os << "\033[" << mod.code << "m";
}
} // namespace color
} // namespace execHelper
