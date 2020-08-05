#ifndef LOGGER_INCLUDE
#define LOGGER_INCLUDE

#include <string_view>

#include "log/log.h"

BOOST_LOG_GLOBAL_LOGGER(exec_helper_core_logger, execHelper::log::LoggerType)

namespace execHelper::log::detail {
using namespace std::literals;
constexpr auto LOG_CHANNEL = "core"sv;
} // namespace execHelper::log::detail

#define LOG(x)                                                                 \
    BOOST_LOG_STREAM_CHANNEL_SEV(exec_helper_core_logger::get(),               \
                                 execHelper::log::detail::LOG_CHANNEL,         \
                                 execHelper::log::x)                           \
        << boost::log::add_value(fileLog, __FILE__)                            \
        << boost::log::add_value(lineLog, __LINE__)

#endif /* LOGGER_INCLUDE */
