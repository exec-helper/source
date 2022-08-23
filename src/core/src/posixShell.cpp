#include "posixShell.h"
#include "log/log.h"

#include <span>
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

#include "config/envp.h"
#include "log/assertions.h"

#include "logger.h"
#include "task.h"

#ifdef _WIN32
using boost::replace_all;
#endif

using std::exception;
using std::span;
using std::string;

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
inline auto getPath(const process::environment& env,
                    const filesystem::path& workingDir) noexcept
    -> std::vector<filesystem::path> {
    std::vector<filesystem::path> path;

    try {
        path.emplace_back(filesystem::absolute(workingDir));
    } catch(const exception& e) {
        LOG(error) << "Error retrieving absolute path to the current working "
                      "directory: "
                   << e.what();
        return {};
    }

    try {
        std::vector<std::string> stringPaths = env.at("PATH").to_vector();
        path.reserve(path.size() + stringPaths.size());
        std::move(stringPaths.begin(), stringPaths.end(),
                  std::back_inserter(path));
        LOG(debug) << "Added current path to the PATH environment variable";
    } catch(const exception&) {
        LOG(debug) << "PATH environment variable does not exist. Adding the "
                      "path of this executable to the PATH!";
        try {
            auto parent_path = this_process::path();
            path.insert(path.end(), parent_path.begin(), parent_path.end());
            return path;
        } catch(const exception& e) {
            LOG(error) << "Failed to retrieve the path to the current binary: "
                       << e.what();
            return {};
        }
    }
    return path;
}
} // namespace

namespace execHelper::core {
auto PosixShell::execute(const Task& task) const
    -> PosixShell::ShellReturnCode {
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

        if(binary.empty()) {
            throw PathNotFoundError(
                std::string("Failed to convert binary '")
                    .append(args.front())
                    .append("' to an absolute path. Are you sure the binary is "
                            "in your PATH?"));
        }

        LOG(debug) << "Binary is relative. Corrected to " << binary;
        binary = filesystem::absolute(binary);
        LOG(debug) << "Binary " << args.front() << " is relative. Corrected to "
                   << binary;
    }
    if(!filesystem::exists(binary)) {
        throw PathNotFoundError(std::string("Could not find binary '")
                                    .append(args.front())
                                    .append("' on this system"));
    }
    if(!filesystem::is_regular_file(binary) &&
       !filesystem::is_symlink(binary)) {
        throw PathNotFoundError(std::string("Cannot execute a non-file binary ")
                                    .append(binary.string()));
    }

    args.erase(args.begin());

    try {
        return system(binary, process::args = args,
                      process::start_dir =
                          filesystem::path(task.getWorkingDirectory()),
                      env);
    } catch(const boost::process::process_error& e) {
        LOG(error) << "Failed to execute command with " << binary;
        user_feedback_error("Failed to execute command. Are you sure "
                            << binary << " is available for this user?'");
        return 1U;
    }
}

auto PosixShell::isExecutedSuccessfully(
    ShellReturnCode returnCode) const noexcept -> bool {
    return returnCode == POSIX_SUCCESS;
}

inline auto PosixShell::shellExpand(const Task& task) noexcept
    -> TaskCollection {
    return wordExpand(task);
}

inline auto PosixShell::wordExpand(const Task& task) noexcept
    -> TaskCollection {
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
    auto envp = Envp(task.getEnvironment());
    environ = envp.getEnvp();

    TaskCollection result;
    for(const auto& taskItem : task.getTask()) {
        wordexp_t expanded{};
        size_t returnCode =
            wordexp(taskItem.c_str(), // NOLINT(concurrency-mt-unsafe
                    &expanded,        // NOLINT(concurrency-mt-unsafe)
                    WRDE_SHOWERR | WRDE_UNDEF);
        if(returnCode == 0) {
            std::span<char*> word(expanded.we_wordv, expanded.we_wordc);
            std::copy(word.begin(), word.end(), std::back_inserter(result));
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
        wordfree(&expanded);
    }
    environ = cached_environ;
    return result;
#endif
}
} // namespace execHelper::core
