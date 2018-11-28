#include <map>
#include <sstream>
#include <string>

#define BOOST_LOG_DYN_LINK 1
#include <boost/core/null_deleter.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/xpressive/xpressive.hpp>

#include "log/logger.h"
#include "unittest/catch.h"
#include "unittest/generators.h"
#include "unittest/rapidcheck.h"

using std::cout;
using std::endl;
using std::exception;
using std::map;
using std::move;
using std::ostream;
using std::string;
using std::stringbuf;
using std::stringstream;
using std::terminate;

using boost::xpressive::smatch;
using boost::xpressive::sregex;

using execHelper::log::all;
using execHelper::log::debug;
using execHelper::log::error;
using execHelper::log::fatal;
using execHelper::log::info;
using execHelper::log::none;
using execHelper::log::trace;
using execHelper::log::warning;

using execHelper::log::Channel;
using execHelper::log::getLogLevels;
using execHelper::log::InvalidLogLevel;
using execHelper::log::LogLevel;
using execHelper::log::toLogLevel;
using execHelper::log::toString;

using execHelper::test::propertyTest;

namespace {
const map<LogLevel, string>& getLogLevelStringMapping() {
    try {
        static const map<LogLevel, string> LOG_LEVEL_STRINGS(
            {{none, "none"},
             {fatal, "fatal"},
             {error, "error"},
             {warning, "warning"},
             {info, "info"},
             {debug, "debug"},
             {trace, "trace"},
             {all, "all"}});
        return LOG_LEVEL_STRINGS;
    } catch(exception& e) {
        cout << e.what() << endl;
    }
    terminate();
}

struct LogMessage {
    string date;
    string time;
    Channel channel;
    LogLevel level;
    string file;
    unsigned int lineNumber;
    string message;

    LogMessage(string date, string time, Channel channel, LogLevel level,
               string file, unsigned int lineNumber, string message)
        : date(move(date)),
          time(move(time)),
          channel(move(channel)),
          level(level),
          file(move(file)),
          lineNumber(lineNumber),
          message(move(message)) {
        ;
    }
};

LogMessage toMessage(const string& message) {
    sregex regex = sregex::compile(
        R"((?P<date>\S*)\s(?P<time>\S*)\s<(?P<severity>\S*)>\s\[(?P<channel>\S*)\]\s(?P<file>.*):(?P<lineNumber>\d*)\s(?P<message>.*)\s$)");
    smatch parsedResult;
    if(regex_search(message, parsedResult, regex)) {
        string lineNumberString =
            parsedResult["lineNumber"]; // Using lexical_cast to a non-string
                                        // type on the match type directly
                                        // results in undefined behaviour. so
                                        // convert it to an intermediate string
                                        // first.
        return LogMessage(
            parsedResult["date"], parsedResult["time"], parsedResult["channel"],
            toLogLevel(parsedResult["severity"]), parsedResult["file"],
            boost::lexical_cast<unsigned int>(lineNumberString),
            parsedResult["message"]);
    }
    return LogMessage("", "", "", none, "", 0U, "");
}

LogLevel getRelativeLogLevel(LogLevel currentLogLevel,
                             int relativeLevel) noexcept {
    int newLogLevel = static_cast<int>(currentLogLevel) + relativeLevel;
    // Saturate at the edges
    if(newLogLevel < all) {
        newLogLevel = all;
    }
    if(newLogLevel > none) {
        newLogLevel = none;
    }
    return static_cast<LogLevel>(newLogLevel);
}
} // namespace

namespace execHelper {
namespace config {
namespace test {
SCENARIO("Test the conversion of correct a log level to a string", "[log]") {
    REQUIRE(
        getLogLevels().size() ==
        getLogLevelStringMapping()
            .size()); // Make sure the log level mappings have at least the same size

    propertyTest(
        "A log level should be converted to the correct string",
        [](LogLevel severity) {
            const auto& logLevelMapping = getLogLevelStringMapping();
            REQUIRE(logLevelMapping.count(severity) == 1U);

            THEN_WHEN("We convert the severity to a string") {
                THEN_CHECK("We should get the right string") {
                    REQUIRE(toString(severity) == logLevelMapping.at(severity));
                }
            }

            THEN_WHEN("We stream the severity") {
                std::stringstream stream;
                stream << severity;
                THEN_CHECK("We should get the right string") {
                    REQUIRE(stream.str() == logLevelMapping.at(severity));
                }
            }
        });
}

SCENARIO("Test the conversion of a correct string to a log level", "[log]") {
    REQUIRE(
        getLogLevels().size() ==
        getLogLevelStringMapping()
            .size()); // Make sure the log level mappings have at least the same size

    propertyTest("A log level string should be converted to the correct level",
                 [](LogLevel severity) {
                     const auto& logLevelMapping = getLogLevelStringMapping();
                     REQUIRE(logLevelMapping.count(severity) == 1U);

                     THEN_WHEN("We convert the string to a severity") {
                         auto actualSeverity =
                             toLogLevel(logLevelMapping.at(severity));

                         THEN_CHECK("We should get the right severity") {
                             REQUIRE(severity == actualSeverity);
                         }
                     }
                 });
}

SCENARIO("Test the conversion of a wrong string to a log level", "[log]") {
    REQUIRE(
        getLogLevels().size() ==
        getLogLevelStringMapping()
            .size()); // Make sure the log level mappings have at least the same size

    propertyTest(
        "A wrong log level string should not be converted to a log level",
        [](const std::string& logLevelString) {
            // Make sure the received string is invalid
            for(const auto& logLevelPair : getLogLevelStringMapping()) {
                RC_PRE(logLevelPair.second != logLevelString);
            }

            THEN_WHEN("We convert the string to a log level") {
                THEN_CHECK("We should get an exception") {
                    REQUIRE_THROWS_AS(toLogLevel(logLevelString),
                                      InvalidLogLevel);
                }
            }
        });
}

SCENARIO("Write a log message with the severity enabled", "[log]") {
    propertyTest("An enabled log level should give the right output",
                 [](LogLevel severity) {
                     stringbuf logBuffer;
                     ostream logStream(&logBuffer);

                     execHelper::log::LogInit logInit(logStream);

                     const string message1("Hello world!!!");

                     THEN_WHEN("We switch on the right severity") {
                         logInit.setSeverity(LOG_CHANNEL, severity);
                         logInit.setSeverity("other-channel",
                                             getRelativeLogLevel(severity, 1));

                         // Switch off clang-format, since it will put the line assignment
                         // on a separate line, annihilating the purpose of the test
                         // clang-format off
            LOG(severity) << message1; const unsigned int line = __LINE__;
                         // clang-format on

                         THEN_CHECK(
                             "We should find the message in the stream") {
                             LogMessage result = toMessage(logBuffer.str());
                             REQUIRE(result.channel == LOG_CHANNEL);
                             REQUIRE(result.level == severity);
                             REQUIRE(result.file == __FILE__);
                             REQUIRE(result.lineNumber == line);
                             REQUIRE(result.message == message1);
                         }
                     }
                 });
}

SCENARIO("Write a log message with the severity disabled", "[log]") {
    propertyTest(
        "A disabled log level should have no output", [](LogLevel severity) {
            RC_PRE(severity != none);

            stringbuf logBuffer;
            ostream logStream(&logBuffer);

            execHelper::log::LogInit logInit(logStream);

            const string message1("Hello world!!!");

            THEN_WHEN("We disable the right severity") {
                logInit.setSeverity(LOG_CHANNEL,
                                    getRelativeLogLevel(severity, 1));
                logInit.setSeverity("other-channel", severity);

                LOG(severity) << message1;

                THEN_CHECK("We should not find the message in the stream") {
                    REQUIRE(logBuffer.in_avail() == 0);
                    REQUIRE(logBuffer.str().empty());
                }
            }
        });
}
} // namespace test
} // namespace config
} // namespace execHelper
