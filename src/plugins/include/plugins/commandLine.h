#ifndef COMMAND_LINE_INCLUDE
#define COMMAND_LINE_INCLUDE

#include <string>
#include <vector>

#include <gsl/string_span>

namespace execHelper {
namespace plugins {
using CommandLineArg = std::string;
using CommandLineArgs = std::vector<CommandLineArg>;

static const gsl::czstring<> COMMAND_LINE_KEY = "command-line";

} // namespace plugins
} // namespace execHelper

#endif /* COMMAND_LINE_INCLUDE */
