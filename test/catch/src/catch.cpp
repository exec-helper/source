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

namespace {
struct ConsoleListener : Catch::TestEventListenerBase {
  public:
    explicit ConsoleListener(const Catch::ReporterConfig& config) noexcept
        : Catch::TestEventListenerBase::TestEventListenerBase(config),
          m_stream(
              std::cout
                  .rdbuf()), // Duplicate the current cout backend (should stil be stdout at this point) to a separate stream, as std::cout itself can be taken over by a reporter
          m_reporterConfig(config.fullConfig(), m_stream),
          m_reporter(m_reporterConfig),
          m_groupInfo("unittest", 0, 1) {
        ;
    }

    void testRunStarting(Catch::TestRunInfo const& _testRunInfo) override {
        m_reporter.testRunStarting(_testRunInfo);
        m_reporter.testGroupStarting(m_groupInfo);
    }

    void testRunEnded(Catch::TestRunStats const& testRunStats) override {
        m_reporter.testGroupEnded(m_groupInfo);
        m_reporter.testRunEnded(testRunStats);
    }

    void skipTest(Catch::TestCaseInfo const& testInfo) override {
        m_reporter.skipTest(testInfo);
    }

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override {
        m_reporter.testCaseStarting(testInfo);
    }

    void testCaseEnded(Catch::TestCaseStats const& testCaseStats) override {
        m_reporter.testCaseEnded(testCaseStats);
    }

    void sectionStarting(Catch::SectionInfo const& sectionInfo) override {
        m_reporter.sectionStarting(sectionInfo);
    }

    void sectionEnded(Catch::SectionStats const& sectionStats) override {
        m_reporter.sectionEnded(sectionStats);
    }

    void assertionStarting(Catch::AssertionInfo const& assertionInfo) override {
        m_reporter.assertionStarting(assertionInfo);
    }

    bool assertionEnded(Catch::AssertionStats const& assertionStats) override {
        return m_reporter.assertionEnded(assertionStats);
    }

  private:
    std::ostream m_stream;
    Catch::ReporterConfig m_reporterConfig;
    Catch::ConsoleReporter m_reporter;
    Catch::GroupInfo m_groupInfo;
};
CATCH_REGISTER_LISTENER(ConsoleListener)
} // namespace

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
        logInit.setSeverity(logModule, logLevel);
    }

    return session.run();
}
