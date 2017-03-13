#include <cstdlib>
#include <string>
#include <fstream>

#include <boost/algorithm/string.hpp>

#include "log/log.h"
#include "core/execHelperOptions.h"
#include "core/posixShell.h"
#include "core/executorInterface.h"
#include "core/immediateExecutor.h"
#include "core/reportingExecutor.h"
#include "core/task.h"
#include "commander/commander.h"

using std::string;
using std::make_pair;
using std::ifstream;
using execHelper::core::ExecHelperOptions;
using execHelper::core::PosixShell;
using execHelper::core::Shell;
using execHelper::core::ExecutorInterface;
using execHelper::core::ImmediateExecutor;
using execHelper::core::ReportingExecutor;
using execHelper::core::EnvironmentCollection;
using execHelper::commander::Commander;

namespace {
    bool fileExist(string filename) {
        ifstream infile(filename);
        return infile.good();
    }

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
}

int execHelperMain(int argc, char** argv, char** envp) {
    ExecHelperOptions options;
    string settingsFile = options.getSettingsFile(argc, argv);
    if(! fileExist(settingsFile)) {
        user_feedback("Could not find a settings file");
        options.printHelp();
        return EXIT_FAILURE;
    }
    if(! options.parseSettingsFile(settingsFile)) {
        user_feedback("Could not parse settings file '" << settingsFile << "'");
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
                 [](Shell::ShellReturnCode /*returnCode*/) { 
                    user_feedback_error("Error executing commands");
                    exit(EXIT_FAILURE);
                 };
    std::unique_ptr<ExecutorInterface> executor;
    if(options.getDryRun()) {
        executor.reset(new ReportingExecutor());
    } else {
        executor.reset(new ImmediateExecutor(shell, callback));
    }
    options.setExecutor(executor.get());

    Commander commander(options, toEnvCollection(envp));
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
