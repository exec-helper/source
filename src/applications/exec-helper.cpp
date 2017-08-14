#include <cstdlib>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional/optional.hpp>

#include "log/log.h"
#include "config/configFileSearcher.h"
#include "core/execHelperOptions.h"
#include "core/posixShell.h"
#include "core/executorInterface.h"
#include "core/immediateExecutor.h"
#include "core/reportingExecutor.h"
#include "core/task.h"
#include "commander/commander.h"

using std::string;
using std::make_pair;

using boost::filesystem::current_path;

using execHelper::config::Path;
using execHelper::config::Paths;
using execHelper::config::ConfigFileSearcher;
using execHelper::core::ExecHelperOptions;
using execHelper::core::PosixShell;
using execHelper::core::Shell;
using execHelper::core::ExecutorInterface;
using execHelper::core::ImmediateExecutor;
using execHelper::core::ReportingExecutor;
using execHelper::core::EnvironmentCollection;
using execHelper::commander::Commander;

namespace {
    inline EnvironmentCollection toEnvCollection(char** envp) {
        static const string DELIMITER("=");
        EnvironmentCollection result;
        size_t index = 0;
        char* envValue;
        while((envValue = envp[index]) != nullptr) {
            string newEnv(envValue); 

            size_t pos = newEnv.find_first_of(DELIMITER);
            assert(pos != newEnv.npos);

            string key = newEnv.substr(0, pos);
            string value = newEnv.substr(pos + DELIMITER.size(), newEnv.npos);
            result.emplace(make_pair(key, value));
            ++index;
        }
        return result;
    }

    inline Paths getSearchPaths(const EnvironmentCollection& env) {
        Paths searchPaths({current_path()});
        const string HOME_DIR_KEY("HOME");
        if(env.count(HOME_DIR_KEY) > 0) {
            searchPaths.push_back(env.at(HOME_DIR_KEY));
        }
        return searchPaths;
    }
}

int execHelperMain(int argc, char** argv, char** envp) {
    ExecHelperOptions options;
    string settingsFileName = options.getSettingsFile(argc, argv);

    EnvironmentCollection env = toEnvCollection(envp);

    ConfigFileSearcher configFileSearcher(getSearchPaths(env));
    boost::optional<Path> settingsFile = configFileSearcher.find(settingsFileName);
    if(settingsFile == boost::none) {
        user_feedback("Could not find a settings file");
        options.printHelp();
        return EXIT_FAILURE;
    }

    if(! options.parseSettingsFile(settingsFile.get().native())) {
        user_feedback("Could not parse settings file '" << settingsFile.get() << "'");
        options.printHelp();
        return EXIT_FAILURE;
    }

    if(! options.parse(argc, argv)) {
        user_feedback("Invalid option");
        options.printHelp();
        return EXIT_FAILURE;
    }

    if(options.containsHelp()) {
        options.printHelp();
        return EXIT_SUCCESS;
    }

    PosixShell shell;
    execHelper::core::ImmediateExecutor::Callback callback = 
                 [](Shell::ShellReturnCode returnCode) { 
                    user_feedback_error("Error executing commands");
                    exit(returnCode);
                 };
    std::unique_ptr<ExecutorInterface> executor;
    if(options.getDryRun()) {
        executor.reset(new ReportingExecutor());
    } else {
        executor.reset(new ImmediateExecutor(shell, callback));
    }
    options.setExecutor(executor.get());

    Commander commander(options, settingsFile.get().parent_path(), std::move(env));
    if(commander.run()) {
        return EXIT_SUCCESS;
    } else {
        user_feedback_error("Error executing commands");
        return EXIT_FAILURE;
    }
}

int main(int argc, char** argv, char** envp) {
    return execHelperMain(argc, argv, envp);
}
