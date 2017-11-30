#include <sstream>
#include <string>
#include <vector>

#define BOOST_LOG_DYN_LINK 1
#include <boost/core/null_deleter.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/xpressive/xpressive.hpp>

#include "log/logger.h"
#include "unittest/catch.h"

using std::cout;
using std::endl;
using std::exception;
using std::move;
using std::ostream;
using std::string;
using std::stringbuf;
using std::stringstream;
using std::terminate;
using std::vector;

using boost::xpressive::smatch;
using boost::xpressive::sregex;

using execHelper::log::all;
using execHelper::log::debug;
using execHelper::log::error;
using execHelper::log::error;
using execHelper::log::fatal;
using execHelper::log::info;
using execHelper::log::none;
using execHelper::log::trace;
using execHelper::log::warning;

using execHelper::log::Channel;
using execHelper::log::InvalidLogLevel;
using execHelper::log::LogLevel;
using execHelper::log::toLogLevel;
using execHelper::log::toString;

namespace {
const vector<LogLevel>& getLogLevels() {
    try {
        static const vector<LogLevel> LOG_LEVELS(
            {none, fatal, error, warning, info, debug, trace, all});
        return LOG_LEVELS;
    } catch(exception& e) {
        cout << e.what() << endl;
    }
    terminate();
}

const vector<string>& getLogLevelStrings() {
    try {
        static const vector<string> LOG_LEVEL_STRINGS(
            {"none", "fatal", "error", "warning", "info", "debug", "trace",
             "all"});
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
    GIVEN("A list of severities") {
        REQUIRE(getLogLevels().size() ==
                getLogLevelStrings().size()); // Make sure the log level
                                              // mappings have at least the same
                                              // size

        auto logString = getLogLevelStrings().begin();
        for(auto logLevel = getLogLevels().begin();
            logLevel != getLogLevels().end(); ++logString, ++logLevel) {
            WHEN("We convert the severity to a string") {
                THEN("We should get the right string") {
                    REQUIRE(toString(*logLevel) == *logString);
                }
            }

            WHEN("We stream the severity") {
                std::stringstream stream;
                stream << *logLevel;
                THEN("We should get the right string") {
                    REQUIRE(stream.str() == *logString);
                }
            }
        }
    }
}

SCENARIO("Test the conversion of a correct string to a log level", "[log]") {
    GIVEN("A list of severities") {
        REQUIRE(getLogLevels().size() ==
                getLogLevelStrings().size()); // Make sure the log level
                                              // mappings have at least the same
                                              // size

        auto logString = getLogLevelStrings().begin();
        for(auto logLevel = getLogLevels().begin();
            logLevel != getLogLevels().end(); ++logString, ++logLevel) {
            WHEN("We convert the string to a severity") {
                THEN("We should get the right severity") {
                    REQUIRE(toLogLevel(*logString) == *logLevel);
                }
            }
        }
    }
}

SCENARIO("Test the conversion of a wrong string to a log level", "[log]") {
    GIVEN("A string that does not map to a log level") {
        const string wrongLogLevel("unknown-level");

        WHEN("We convert the string to a log level") {
            THEN("We should get an exception") {
                REQUIRE_THROWS_AS(toLogLevel(wrongLogLevel), InvalidLogLevel);
            }
        }
    }
}

SCENARIO("Write a log message with the severity enabled", "[log]") {
    GIVEN("A list of severities") {
        for(auto severity : getLogLevels()) {
            const string message1("Hello world!!!");

            WHEN("We switch on the right severity") {
                stringbuf logBuffer;
                ostream logStream(&logBuffer);

                execHelper::log::init(logStream);
                execHelper::log::setSeverity(LOG_CHANNEL, severity);
                execHelper::log::setSeverity("other-channel",
                                             getRelativeLogLevel(severity, 1));

                // Switch off clang-format, since it will put the line assignment
                // on a separate line, annihilating the purpose of the test
                // clang-format off
                LOG(severity) << message1; const unsigned int line = __LINE__;
                // clang-format on

                THEN("We should find the message in the stream") {
                    LogMessage result = toMessage(logBuffer.str());
                    REQUIRE(result.channel == LOG_CHANNEL);
                    REQUIRE(result.level == severity);
                    REQUIRE(result.file == __FILE__);
                    REQUIRE(result.lineNumber == line);
                    REQUIRE(result.message == message1);
                }
            }
        }
    }
}

SCENARIO("Write a log message with the severity disabled", "[log]") {
    GIVEN("A list of severities") {
        for(auto severity : getLogLevels()) {
            if(severity == none) { // Skip the none severity, as we can not
                                   // raise the severity above this level:
                                   // messages that are 'logged' at this level
                                   // will always be shown
                continue;
            }
            const string message1("Hello world!!!");
            WHEN("We disable the right severity") {
                stringbuf logBuffer;
                ostream logStream(&logBuffer);

                execHelper::log::init(logStream);
                execHelper::log::setSeverity(LOG_CHANNEL,
                                             getRelativeLogLevel(severity, 1));
                execHelper::log::setSeverity("other-channel", severity);

                LOG(severity) << message1;

                THEN("We should not find the message in the stream") {
                    REQUIRE(logBuffer.str().empty());
                }
            }
        }
    }
}
} // namespace test
} // namespace config
} // namespace execHelper
