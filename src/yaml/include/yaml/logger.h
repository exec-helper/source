#ifndef LOGGER_INCLUDE
#define LOGGER_INCLUDE

#include <gsl/string_span>

#include "log/log.h"
BOOST_LOG_GLOBAL_LOGGER(
    exec_helper_yaml_logger,
    execHelper::log::LoggerType); // NOLINT(modernize-use-using)

static gsl::czstring<> LOG_CHANNEL = "yaml";
#define LOG(x)                                                                 \
    BOOST_LOG_STREAM_CHANNEL_SEV(exec_helper_yaml_logger::get(), LOG_CHANNEL,  \
                                 execHelper::log::x)                           \
        << boost::log::add_value(fileLog, __FILE__)                            \
        << boost::log::add_value(lineLog, __LINE__)

#endif /* LOGGER_INCLUDE */
