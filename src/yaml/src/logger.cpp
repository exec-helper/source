#include "logger.h"

BOOST_LOG_GLOBAL_LOGGER_INIT( // NOLINT(modernize-use-trailing-return-type)
    exec_helper_yaml_logger, execHelper::log::LoggerType) {
    return execHelper::log::LoggerType(
        boost::log::keywords::
            channel = // NOLINT(fuchsia-default-arguments-calls)
        LOG_CHANNEL);
}
