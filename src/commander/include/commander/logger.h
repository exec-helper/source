#ifndef LOGGER_INCLUDE
#define LOGGER_INCLUDE

#include "log/log.h"
BOOST_LOG_GLOBAL_LOGGER(exec_helper_commander_logger,
                        execHelper::log::LoggerType);

static const std::string LOG_CHANNEL = "commander";
#define LOG(x)                                                                 \
    BOOST_LOG_STREAM_CHANNEL_SEV(exec_helper_commander_logger::get(),          \
                                 LOG_CHANNEL, execHelper::log::x)              \
        << boost::log::add_value(fileLog, __FILE__)                            \
        << boost::log::add_value(lineLog, __LINE__)

#endif /* LOGGER_INCLUDE */
