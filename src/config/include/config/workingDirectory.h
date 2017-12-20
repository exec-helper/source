#ifndef WORKING_DIRECTORY_INCLUDE
#define WORKING_DIRECTORY_INCLUDE

#include <string>

#include <gsl/string_span>

namespace execHelper {
namespace config {
using WorkingDir = std::string;
const gsl::czstring<> WORKING_DIR_KEY = "working-dir";
} // namespace config
} // namespace execHelper

#endif /* WORKING_DIRECTORY_INCLUDE */
