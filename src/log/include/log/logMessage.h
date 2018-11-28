#ifndef LOG_MESSAGE_INCLUDE
#define LOG_MESSAGE_INCLUDE

#define BOOST_LOG_DYN_LINK 1
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/channel_feature.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#include "logLevel.h"

namespace execHelper {
namespace log {
using Channel = std::string;
using LoggerType =
    boost::log::sources::severity_channel_logger_mt<LogLevel, std::string>;
} // namespace log
} // namespace execHelper

BOOST_LOG_ATTRIBUTE_KEYWORD(fileLog, "File",
                            std::string) // NOLINT(modernize-use-using)
BOOST_LOG_ATTRIBUTE_KEYWORD(lineLog, "Line",
                            unsigned int) // NOLINT(modernize-use-using)
BOOST_LOG_ATTRIBUTE_KEYWORD(              // NOLINT(modernize-use-using)
    timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", // NOLINT(modernize-use-using)
                            execHelper::log::LogLevel)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", // NOLINT(modernize-use-using)
                            execHelper::log::Channel)

#endif /* LOG_MESSAGE_INCLUDE */
