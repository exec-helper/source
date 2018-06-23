#include "execution.h"

#include <cassert>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/filesystem/fstream.hpp>

#include "base-utils/executionContent.h"
#include "base-utils/path.h"

#include "unittest/logger.h"

using boost::filesystem::current_path;
using boost::system::error_code;

using execHelper::test::baseUtils::Path;
using execHelper::test::baseUtils::ReturnCode;
using execHelper::test::baseUtils::execution::CommandLine;
using execHelper::test::baseUtils::execution::CommandLineRaw;

namespace {
void childProcessExecute(const CommandLineRaw& line,
                         const Path& workingDir) noexcept {
    ReturnCode returnCode;

    error_code error;
    current_path(workingDir, error);
    assert(error ==
           boost::system::errc::success); // Either works or fix the test

    int fd = ::open("/dev/null",     // NOLINT(cppcoreguidelines-pro-type-vararg, hicpp-vararg)
                    O_WRONLY |       // NOLINT(hicpp-signed-bitwise)
                        O_CLOEXEC);  // NOLINT(hicpp-signed-bitwise)
    assert(fd >= 0);                // Either works or fix the test
    returnCode = dup2(fd, 1);
    if(returnCode == 0) {
        LOG(test)
            << "Error: could not duplicate stdout file descriptor in child "
               "process ("
            << returnCode << ")";
        assert(false); // Either works or fix the test
    }
    close(fd);

    if((returnCode = execvp(line[0], &(line[0]))) == -1) {
        LOG(test) << "Error (" << returnCode
                  << "): Could not execvp command: " << strerror(errno) << " ("
                  << errno << ")";
    }
    // execvp replaces the process: the return code for valid execvp calls should be obtained from the parent
    assert(false); // Either works or fix the test
}

ReturnCode waitForChild(pid_t pid) noexcept {
    pid_t ret;
    ReturnCode status;
    while((ret = waitpid(pid, &status, 0)) == -1) {
        if(errno != EINTR) {
            break;
        }
    }
    assert(ret != -1); // Either works or fix the test
    if(status == 0) {
        return 0;
    }
    if(WIFEXITED(status)) { // NOLINT(hicpp-signed-bitwise)
        return WEXITSTATUS(status); // NOLINT(hicpp-signed-bitwise)
    }
    if(WIFSIGNALED(status)) {   // NOLINT(hicpp-signed-bitwise)
        return WTERMSIG(status);    // NOLINT(hicpp-signed-bitwise)
    }
    return status;
}
} // namespace

namespace execHelper {
namespace test {
namespace baseUtils {
namespace execution {
int execute(const CommandLineRaw& commandLine,
            const Path& workingDir) noexcept {
    pid_t pid = fork();
    assert(pid != -1);
    if(pid == 0) {
        childProcessExecute(commandLine, workingDir);
    }
    return waitForChild(pid);
}

int execute(const CommandLine& commandLine, const Path& workingDir) noexcept {
    CommandLineRaw rawLine;
    rawLine.reserve(commandLine.size());
    for(const auto& arg : commandLine) {
        rawLine.push_back(strndup(arg.c_str(), arg.size()));
    }
    rawLine.push_back(nullptr);
    auto returnCode = execute(rawLine, workingDir);

    // Free the duplicated strings
    for(auto arg : rawLine) {
        free(arg);  // NOLINT(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory, hicpp-no-malloc)
    }
    return returnCode;
}
} // namespace execution
} // namespace baseUtils
} // namespace test
} // namespace execHelper
