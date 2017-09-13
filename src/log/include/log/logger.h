#ifndef LOGGER_INCLUDE
#define LOGGER_INCLUDE

#include <gsl/string_span>

#include "log.h"
BOOST_LOG_GLOBAL_LOGGER(exec_helper_log_logger, execHelper::log::LoggerType);    // NOLINT(modernize-use-using)

static const gsl::czstring<> LOG_CHANNEL="log";
#define LOG(x) BOOST_LOG_STREAM_CHANNEL_SEV(exec_helper_log_logger::get(), LOG_CHANNEL, x) << boost::log::add_value(fileLog, __FILE__) << boost::log::add_value(lineLog, __LINE__)

#endif  /* LOGGER_INCLUDE */
