#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "log/log.h"
#include "log/logLevel.h"

#define CATCH_CONFIG_RUNNER
#include "catch.h"

using std::cerr;
using std::cout;
using std::endl;
using std::move;
using std::string;
using std::vector;

using Catch::Session;
using Catch::clara::Opt;

using execHelper::log::InvalidLogLevel;
using execHelper::log::toLogLevel;
using execHelper::log::LogLevel::none;

int main(int argc, char** argv) {
    Session session;

    std::string logLevelString("none");
    auto cli = session.cli();
    cli |= Opt(logLevelString,
               "level")["-u"]["--debug"]("Set the log level of the binary");
    session.cli(cli);

    int returnCode = session.applyCommandLine(argc, argv);
    if(returnCode != 0) {
        cerr << "Error parsing command line parameters" << endl;
        return returnCode;
    }

    auto logLevel = none;
    try {
        logLevel = toLogLevel(logLevelString);
    } catch(InvalidLogLevel& e) {
        cerr << "Invalid log level string: " << logLevelString << std::endl;
        return EXIT_FAILURE;
    }

    static vector<string> logModules(
        {"log", "yaml", "config", "core", "plugins", "commander"});

    execHelper::log::LogInit logInit;
    for(const auto& logModule : logModules) {
        if(!logInit.setSeverity(logModule, logLevel)) {
            cerr << "Error settings log level '" << logLevel << "' for module '"
                 << logModule << "'" << endl;
            return EXIT_FAILURE;
        }
    }

    return session.run();
}
