#include "posixShell.h"

#include <vector>

#ifdef _WIN32
#include <boost/algorithm/string.hpp>
#else
#include <glob.h>
#include <wordexp.h>
#endif

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/process/start_dir.hpp>
#include <gsl/span>

#include "config/envp.h"
#include "log/assertions.h"

#include "logger.h"
#include "task.h"

#ifdef _WIN32
using boost::replace_all;
#endif

using std::string;

using gsl::span;
using gsl::zstring;

using execHelper::config::Envp;

namespace process = boost::process;
namespace this_process = boost::this_process;
namespace filesystem = boost::filesystem;

namespace {
const execHelper::core::PosixShell::ShellReturnCode POSIX_SUCCESS = 0U;

/**
 * This construction constructs the PATH from its parents' path and the various inputs.
 * \note It is guaranteed that the given working directory will be the first entry in the returned path
 *
 * \param[in] env   The environment to consider for constructing the path
 * \param[in] workingDir    The working directory from where the path will operate
 * \returns The constructed PATH for the child. The first entry will be the given working directory
 */
inline std::vector<filesystem::path>
getPath(const process::environment& env,
        const filesystem::path& workingDir) noexcept {
    std::vector<filesystem::path> path({filesystem::absolute(workingDir)});
    if(env.count("PATH") == 0) {
        auto parent_path = this_process::path();
        path.insert(path.end(), parent_path.begin(), parent_path.end());
        return path;
    }
    std::vector<std::string> stringPaths = env.at("PATH").to_vector();
    path.reserve(stringPaths.size() + stringPaths.size());
    std::copy(stringPaths.begin(), stringPaths.end(), std::back_inserter(path));
    return path;
}
} // namespace

namespace execHelper::core {
PosixShell::ShellReturnCode PosixShell::execute(const Task& task) {
    if(task.getTask().empty()) {
        return POSIX_SUCCESS;
    }

    process::environment env = this_process::
        environment(); // Explicitly copy the environment so that only the child environment is modified
    for(const auto& envPair : task.getEnvironment()) {
        env[envPair.first] = envPair.second;
    }

    TaskCollection args = shellExpand(task);
    filesystem::path binary = args.front();

    if(binary.is_relative()) {
        binary = process::search_path(
            args.front(),
            getPath(
                env,
                filesystem::path(
                    task.getWorkingDirectory()))); // getPath guarantees that the given working directory is part of the PATH it returns, so there is no need to explicitly look for the binary in the considered working directory first.

        binary = filesystem::absolute(binary);
    }
    if(!filesystem::exists(binary)) {
        throw PathNotFoundError(std::string("Could not find binary '")
                                    .append(args.front())
                                    .append("' on this system"));
    }
    args.erase(args.begin());

    return system(
        binary, process::args = args,
        process::start_dir = filesystem::path(task.getWorkingDirectory()), env);
}

bool PosixShell::isExecutedSuccessfully(ShellReturnCode returnCode) const
    noexcept {
    return returnCode == POSIX_SUCCESS;
}

inline TaskCollection PosixShell::shellExpand(const Task& task) noexcept {
    return wordExpand(task);
}

inline TaskCollection PosixShell::wordExpand(const Task& task) noexcept {
#ifdef _WIN32
    TaskCollection result;

    auto environment = task.getEnvironment();

    // Windows has some 'special' environment variables
    environment["cd"] = task.getWorkingDirectory().string();
    environment["CD"] = task.getWorkingDirectory().string();

    for(auto arg : task.getTask()) {
        for(const auto& env : environment) {
            auto pattern = std::string("%").append(env.first).append("%");
            replace_all(arg, pattern, env.second);
        }
        result.emplace_back(arg);
    }
    return result;

#else
    // Cache envp pointer
    char** cached_environ = environ;

    // Set global environment, since this is where wordexp will look for substitutions
    Envp envp = Envp(task.getEnvironment());
    environ = envp.getEnvp();

    TaskCollection result;
    for(const auto& taskItem : task.getTask()) {
        wordexp_t p{};
        size_t returnCode =
            wordexp(taskItem.c_str(), &p, WRDE_SHOWERR | WRDE_UNDEF);
        if(returnCode == 0) {
            span<zstring<>> w(p.we_wordv, p.we_wordc);
            std::copy(w.begin(), w.end(), std::back_inserter(result));
        } else {
            switch(returnCode) {
            case WRDE_BADVAL:
                user_feedback_error("Command contains undefined variable");
                break;
            case WRDE_SYNTAX:
                user_feedback_error("Syntax error in shell argument");
                break;
            case WRDE_BADCHAR:
                user_feedback_error("Bad character: Illegal occurrence of "
                                    "newline or one of |, &, ;, <, >, (, ), {, "
                                    "}.");
                break;
            default:
                user_feedback_error(
                    "An error occured during executing: " << returnCode);
                break;
            }
            result.push_back(taskItem);
        }
        wordfree(&p);
    }
    environ = cached_environ;
    return result;
#endif
}
} // namespace execHelper::core
