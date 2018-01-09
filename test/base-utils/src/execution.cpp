#include "execution.h"

#include <cassert>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/filesystem/fstream.hpp>

#include "base-utils/executionContent.h"
#include "base-utils/path.h"

using std::cout;
using std::endl;

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

    int fd = ::open("/dev/null",
                    O_WRONLY |
                        O_CLOEXEC); // NOLINT(cppcoreguidelines-pro-type-vararg)
    assert(fd >= 0);                // Either works or fix the test
    returnCode = dup2(fd, 1);
    if(returnCode == 0) {
        cout << "Error: could not duplicate stdout file descriptor in child "
                "process ("
             << returnCode << ")" << endl;
        assert(false); // Either works or fix the test
    }
    close(fd);

    if((returnCode = execvp(line[0], &(line[0]))) == -1) {
        cout << "Error (" << returnCode
             << "): Could not execvp command: " << strerror(errno) << " ("
             << errno << ")" << endl;
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
    if(WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if(WIFSIGNALED(status)) {
        return WTERMSIG(status);
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
    bool returnCode = execute(rawLine, workingDir);

    // Free the duplicated strings
    for(auto arg : rawLine) {
        free(arg);
    }
    return returnCode;
}
} // namespace execution
} // namespace baseUtils
} // namespace test
} // namespace execHelper
