#include "execution.h"

#include <cassert>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <gsl/gsl>

#include "base-utils/executionContent.h"
#include "base-utils/path.h"

#include "logger.h"

namespace bp = boost::process;

using boost::filesystem::exists;
using boost::process::search_path;
using boost::process::system;

using execHelper::test::baseUtils::Path;
using execHelper::test::baseUtils::ReturnCode;
using execHelper::test::baseUtils::execution::CommandLine;

namespace execHelper::test::baseUtils::execution {
auto execute(const CommandLine& commandLine, const Path& workingDir) noexcept -> int {
    boost::filesystem::path binary = commandLine.front();
    if(! exists(binary)) {
        binary = search_path(binary);
        if(binary.empty()) {
            LOG(error) << "Could not find binary '" << binary.native()
                       << "' on this system";
            Ensures(false);
        }
    }
    CommandLine args;
    args.reserve(commandLine.size() - 1);
    args.insert(args.end(), commandLine.begin() + 1, commandLine.end());

    return system(binary, bp::args = args,
                  bp::start_dir = boost::filesystem::path(workingDir));
}
} // namespace execHelper::test::baseUtils::execution
