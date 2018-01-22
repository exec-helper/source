#include "execution.h"

#include <cassert>
#include <fcntl.h>
#include <filesystem>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include "base-utils/executionContent.h"
#include "base-utils/path.h"

#include "unittest/logger.h"

namespace bp = boost::process;
namespace filesystem = boost::filesystem;

using boost::filesystem::exists;
using boost::process::search_path;
using boost::process::system;
using boost::system::error_code;

using execHelper::test::baseUtils::Path;
using execHelper::test::baseUtils::ReturnCode;
using execHelper::test::baseUtils::execution::CommandLine;

namespace execHelper {
namespace test {
namespace baseUtils {
namespace execution {
int execute(const CommandLine& commandLine, const Path& workingDir) noexcept {
    filesystem::path binary = commandLine.front();
    if(!filesystem::exists(binary)) {
        binary = search_path(binary);
        if(binary.empty()) {
            LOG(error) << "Could not find binary '" << binary.native()
                       << "' on this system";
            assert(false); // Fix the test
        }
    }
    CommandLine args;
    args.reserve(commandLine.size() - 1);
    args.insert(args.end(), commandLine.begin() + 1, commandLine.end());

    return system(binary, bp::args = args,
                  bp::start_dir = filesystem::path(workingDir));
}
} // namespace execution
} // namespace baseUtils
} // namespace test
} // namespace execHelper
