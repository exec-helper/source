#include "posixShell.h"

#include <cerrno>
#include <cstring>
#include <glob.h>
#include <limits> 
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <wordexp.h>

#include <boost/filesystem.hpp>
#include <gsl/span>
#include <gsl/string_span>

#include "log/assertions.h"

#include "argv.h"
#include "envp.h"
#include "logger.h"
#include "task.h"

using std::string;
using std::numeric_limits;

using boost::filesystem::current_path;
using boost::system::error_code;
using boost::system::errc::success;
using gsl::zstring;
using gsl::span;

namespace {
    const execHelper::core::PosixShell::ShellReturnCode POSIX_SUCCESS = 0U;
} // namespace

namespace execHelper { namespace core {
    PosixShell::ShellReturnCode PosixShell::execute(const Task& task) noexcept {
        pid_t pid = fork();
        if(pid == -1) {
            return std::numeric_limits<PosixShell::ShellReturnCode>::max();
        }
        if(pid == 0) {
            childProcessExecute(task);
        }
        return waitForChild(pid);
    }

    bool PosixShell::isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept {
        return returnCode == POSIX_SUCCESS;
    }

    void PosixShell::childProcessExecute(const Task& task) const noexcept {
        int returnCode;

        // Change to the correct working directory
        error_code error;
        LOG(info) << "Changing to directory " << task.getWorkingDirectory() << "...";
        current_path(task.getWorkingDirectory(), error);
        if(error != success) {
           LOG(error) << "Could not change to directory " << task.getWorkingDirectory() << " (" << error << ")";
            _exit(127);
        }

        TaskCollection taskCollection = shellExpand(task);
        Envp envp(task.getEnvironment());
        LOG(debug) << R"(Environment: ")" << envp << R"(")";

        Argv argv(taskCollection);
        LOG(debug) << R"(Executing ")" << argv << R"(")";

        // A full copy of m_env is not required, since it is used in a separate process
        if ((returnCode = execvpe(argv[0], argv.getArgv(), envp.getEnvp())) == -1) {
            LOG(error) << "Could not execvpe command: " << strerror(errno) << " (" << errno << ")"; 
        }

        // execvp only returns if something goes wrong
        expects(false);
        _exit(127);
    }

    PosixShell::ShellReturnCode PosixShell::waitForChild(pid_t pid) const noexcept {
        pid_t ret;
        int status;
        while ((ret = waitpid(pid, &status, 0)) == -1) {
            if (errno != EINTR) {
                LOG(error) << "Child exited with error state";
                break;
            }
        }
        if(ret == -1) {
            LOG(error) << "Error executing command";
            return std::numeric_limits<PosixShell::ShellReturnCode>::max();
        }
        if(status == 0) {
            return POSIX_SUCCESS;
        }
        if (WIFEXITED(status) && WEXITSTATUS(status)) {
            return WEXITSTATUS(status);
        }
        LOG(error) << "Child exited with unexpected child status: " << status;
        return status;
    }

    inline TaskCollection PosixShell::shellExpand(const Task& task) noexcept {
        return wordExpand(task);
    }

    inline TaskCollection PosixShell::wordExpand(const Task& task) noexcept {
        // Cache envp pointer
        char** cached_environ = environ;

        // Set global environment, since this is where wordexp will look for substitutions
        Envp envp = Envp(task.getEnvironment());
        environ = envp.getEnvp();

        TaskCollection result;
        for(const auto& taskItem : task.getTask()) {
            wordexp_t p{};
            size_t returnCode = wordexp(taskItem.c_str(), &p, WRDE_SHOWERR | WRDE_UNDEF);
            if(returnCode == 0) {
                span<zstring<>> w(p.we_wordv, p.we_wordc);
                for(const auto& expandedWord : w) {
                    result.emplace_back(expandedWord);
                }
            } else {
                switch(returnCode) {
                    case WRDE_BADVAL:
                        user_feedback_error("Command contains undefined variable");
                        break;
                    case WRDE_SYNTAX:
                        user_feedback_error("Syntax error in shell argument");
                        break;
                    case WRDE_BADCHAR:
                        user_feedback_error("Bad character: Illegal occurrence of newline or one of |, &, ;, <, >, (, ), {, }.");
                        break;
                    default:
                        user_feedback_error("An error occured during executing: " << returnCode);
                        break;
                }
                result.push_back(taskItem);
            }
            wordfree(&p);
        }
        environ = cached_environ;
        return result;
    }
} // namespace core
} // namespace execHelper
