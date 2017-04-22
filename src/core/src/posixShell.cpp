#include "posixShell.h"

#include <cerrno>
#include <cstring>
#include <glob.h>
#include <limits> 
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <wordexp.h>

#include "log/log.h"

#include "argv.h"
#include "envp.h"
#include "task.h"

using std::string;
using std::numeric_limits;

namespace {
    const execHelper::core::PosixShell::ShellReturnCode POSIX_SUCCESS = 0U;

}

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

        TaskCollection taskCollection = shellExpand(task.getTask());
        Argv argv(taskCollection);
        Envp envp(task.getEnvironment());

        // A full copy of m_env is not required, since it is used in a separate process
        if ((returnCode = execvpe(argv[0], argv.getArgv(), envp.getEnvp())) == -1) {
            LOG("Could not execvpe command: " << strerror(errno) << " (" << errno << ")"); 
        }
        // execvp only returns if something goes wrong
        // Delete it, though we are going to free it anyway once we exit
        _exit(127);
    }

    PosixShell::ShellReturnCode PosixShell::waitForChild(pid_t pid) const noexcept {
        pid_t ret;
        int status;
        while ((ret = waitpid(pid, &status, 0)) == -1) {
            if (errno != EINTR) {
                LOG("Child exited with error state");
                break;
            }
        }
        if(ret == -1) {
            LOG("Error executing command");
            return std::numeric_limits<PosixShell::ShellReturnCode>::max();
        }
        if(status == 0) {
            return POSIX_SUCCESS;
        }
        if (WIFEXITED(status) && WEXITSTATUS(status)) {
            return WEXITSTATUS(status);
        }
        LOG("Child exited with unexpected child status: " << status);
        return status;
    }

    inline TaskCollection PosixShell::shellExpand(const TaskCollection& task) noexcept {
        return wordExpand(task);
    }

    inline TaskCollection PosixShell::wordExpand(const TaskCollection& task) noexcept {
        TaskCollection result;
        for(const auto& taskItem : task) {
            wordexp_t p{};
            size_t returnCode = wordexp(taskItem.c_str(), &p, WRDE_SHOWERR | WRDE_UNDEF);
            if(returnCode == 0) {
                char** w = p.we_wordv;
                for (size_t i = 0; i < p.we_wordc; i++) {
                    string expandedWord(w[i]);
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
        return result;
    }
} }
