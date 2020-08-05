#include "logger.h"

BOOST_LOG_GLOBAL_LOGGER_INIT(exec_helper_commander_logger,
                             execHelper::log::LoggerType) {
    return execHelper::log::LoggerType(
        boost::log::keywords::channel = execHelper::log::detail::LOG_CHANNEL);
}
