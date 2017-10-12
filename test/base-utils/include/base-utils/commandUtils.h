#ifndef COMMAND_UTILS_INCLUDE
#define COMMAND_UTILS_INCLUDE

#include <limits>
#include <string>
#include <vector>

#include <gsl/string_span>

#include "tmpFile.h"

namespace execHelper {
    namespace test {
        namespace baseUtils {
            using ConfigFile = TmpFile;

            const gsl::czstring<> EXEC_HELPER_BINARY="exec-helper";
            const gsl::czstring<> COMMAND_KEY = "commands";
            const gsl::czstring<> COMMAND_LINE_COMMAND_KEY = "command-line-command";
            const gsl::czstring<> COMMAND_LINE_COMMAND_LINE_KEY = "command-line";

            using ReturnCode = int32_t;
            static const ReturnCode SUCCESS = EXIT_SUCCESS;
            static const ReturnCode RUNTIME_ERROR = std::numeric_limits<ReturnCode>::max();
        } // namespace baseUtils
    } // namespace test
} // namespace execHelper

#endif  /* COMMAND_UTILS_INCLUDE */
