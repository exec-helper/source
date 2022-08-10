#ifndef COMMAND_UTILS_INCLUDE
#define COMMAND_UTILS_INCLUDE

#include <limits>
#include <string>
#include <vector>

#include "tmpFile.h"

namespace execHelper::test::baseUtils {
using ConfigFile = TmpFile;

using namespace std::literals;

constexpr auto EXEC_HELPER_BINARY = "exec-helper"sv;
constexpr auto COMMAND_KEY = "commands"sv;
constexpr auto COMMAND_LINE_COMMAND_KEY = "command-line-command"sv;
constexpr auto COMMAND_LINE_COMMAND_LINE_KEY = "command-line"sv;

using ReturnCode = int32_t;
static const ReturnCode SUCCESS = EXIT_SUCCESS;
static const ReturnCode RUNTIME_ERROR = std::numeric_limits<ReturnCode>::max();
} // namespace execHelper::test::baseUtils

#endif /* COMMAND_UTILS_INCLUDE */
