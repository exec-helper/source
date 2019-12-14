#include <map>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/core/null_deleter.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/xpressive/xpressive.hpp>

#include "log/generators.h"
#include "log/logLevel.h"
#include "log/logger.h"
#include "unittest/catch.h"
#include "unittest/rapidcheck.h"

using std::cout;
using std::endl;
using std::exception;
using std::map;
using std::move;
using std::ostream;
using std::string;
using std::string_view;
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
auto getLogLevelStringMapping() -> const map<LogLevel, string_view>& {
    try {
        using namespace std::literals;
        static const map<LogLevel, string_view> LOG_LEVEL_STRINGS(
            {{none, "none"sv},
             {fatal, "fatal"sv},
             {error, "error"sv},
             {warning, "warning"sv},
             {info, "info"sv},
             {debug, "debug"sv},
             {trace, "trace"sv},
             {all, "all"sv}});
        return LOG_LEVEL_STRINGS;
    } catch(exception& e) {
        cout << e.what() << endl;
    }
    terminate();
}

class LogMessage {
  public:
    LogMessage(string date, string time, Channel channel, LogLevel level,
               string file, unsigned int lineNumber, string message)
        : m_date(move(date)),
          m_time(move(time)),
          m_channel(move(channel)),
          m_level(level),
          m_file(move(file)),
          m_lineNumber(lineNumber),
          m_message(move(message)) {
        ;
    }

    [[nodiscard]] auto getDate() const noexcept -> const string& {
        return m_date;
    }

    [[nodiscard]] auto getTime() const noexcept -> const string& {
        return m_time;
    }

    [[nodiscard]] auto getChannel() const noexcept -> const Channel& {
        return m_channel;
    }

    [[nodiscard]] auto getLevel() const noexcept -> const LogLevel& {
        return m_level;
    }

    [[nodiscard]] auto getFile() const noexcept -> const string& {
        return m_file;
    }

    [[nodiscard]] auto getLineNumber() const noexcept -> unsigned int {
        return m_lineNumber;
    }

    [[nodiscard]] auto getMessage() const noexcept -> const string& {
        return m_message;
    }

  private:
    string m_date;
    string m_time;
    Channel m_channel;
    LogLevel m_level;
    string m_file;
    unsigned int m_lineNumber;
    string m_message;
};

auto toMessage(const string& message) -> LogMessage {
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
        string logLevelString = parsedResult["severity"];
        LogLevel logLevel = toLogLevel(logLevelString);
        return LogMessage(parsedResult["date"], parsedResult["time"],
                          parsedResult["channel"], logLevel,
                          parsedResult["file"],
                          boost::lexical_cast<unsigned int>(lineNumberString),
                          parsedResult["message"]);
    }
    return LogMessage("", "", "", none, "", 0U, "");
}

auto getRelativeLogLevel(LogLevel currentLogLevel, int relativeLevel) noexcept
    -> LogLevel {
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

namespace execHelper::config::test {
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
                             REQUIRE(result.getChannel() == LOG_CHANNEL);
                             REQUIRE(result.getLevel() == severity);
                             REQUIRE(result.getFile() == __FILE__);
                             REQUIRE(result.getLineNumber() == line);
                             REQUIRE(result.getMessage() == message1);
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
} // namespace execHelper::config::test
