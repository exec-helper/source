#include "logger.h"

BOOST_LOG_GLOBAL_LOGGER_INIT( // NOLINT(modernize-use-trailing-return-type)
    exec_helper_test_logger, execHelper::log::LoggerType) {
    return execHelper::log::LoggerType(boost::log::keywords::channel =
                                           LOG_CHANNEL);
}
