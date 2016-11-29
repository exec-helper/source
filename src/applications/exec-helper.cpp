#include <cstdlib>
#include <string>
#include <fstream>

#include "log/log.h"
#include "core/execHelperOptions.h"
#include "core/posixShell.h"
#include "core/immediateExecutor.h"
#include "commander/commander.h"

using std::string;
using std::ifstream;
using execHelper::core::ExecHelperOptions;
using execHelper::core::PosixShell;
using execHelper::core::ImmediateExecutor;
using execHelper::commander::Commander;

namespace {
    bool fileExist(string filename) {
        ifstream infile(filename);
        return infile.good();
    }
}

int execHelperMain(int argc, char** argv) {
    ExecHelperOptions options;
    if(! options.parse(argc, argv)) {
        user_feedback("Invalid option");
        options.printHelp();
        return EXIT_FAILURE;
    }
    if(options.containsHelp()) {
        options.printHelp();
        return EXIT_SUCCESS;
    }

    string settingsFile = options.getSettingsFile(argc, argv);
    if(! fileExist(settingsFile)) {
        user_feedback("Could not find a settings file");
        return EXIT_FAILURE;
    }
    if(! options.parseSettingsFile(settingsFile)) {
        user_feedback("Could not parse settings file '" << settingsFile << "'");
        return EXIT_FAILURE;
    }
    if(! options.parse(argc, argv)) {
        user_feedback("Invalid option");
        options.printHelp();
        return EXIT_FAILURE;
    }

    PosixShell shell;
    ImmediateExecutor executor(shell);
    options.setExecutor(&executor);

    Commander commander(options);
    commander.run();

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    return execHelperMain(argc, argv);
}
