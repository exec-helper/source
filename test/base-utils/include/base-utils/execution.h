#ifndef EXECUTE_INCLUDE
#define EXECUTE_INCLUDE

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <gsl/string_span>

#include "path.h"

namespace execHelper {
namespace test {
namespace baseUtils {
namespace execution {
using CommandLineRaw = std::vector<gsl::zstring<>>;
using CommandLine = std::vector<std::string>;

int execute(
    const CommandLineRaw& commandLine,
    const Path& workingDir = boost::filesystem::current_path()) noexcept;
int execute(
    const CommandLine& commandLine,
    const Path& workingDir = boost::filesystem::current_path()) noexcept;
} // namespace execution
} // namespace baseUtils
} // namespace test
} // namespace execHelper

#endif /* EXECUTE_INCLUDE */
