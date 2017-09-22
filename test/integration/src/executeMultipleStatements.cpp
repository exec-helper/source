#include "unittest/catch.h"

#include <fcntl.h>
#include <iostream>
#include <map>
#include <string>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include <boost/filesystem.hpp>
#include <gsl/string_span>

#include "base-utils/configFileWriter.h"
#include "log/assertions.h"
#include "base-utils/executionContent.h"
#include "base-utils/executionHandler.h"
#include "base-utils/tmpFile.h"

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::stringstream;
using std::to_string;
using std::vector;

using boost::filesystem::current_path;
using boost::system::error_code;
using boost::system::errc::success;
using gsl::czstring;
using gsl::zstring;

using execHelper::test::baseUtils::ConfigFileWriter;
using execHelper::test::baseUtils::ExecutionContent;
using execHelper::test::baseUtils::ExecutionHandler;
using execHelper::test::baseUtils::TmpFile;
using execHelper::test::baseUtils::YamlWriter;

namespace {
    using Command = string;
    using Commands = vector<string>;
    using CommandLineRaw = vector<zstring<>>;
    using CommandLine = vector<string>;
    using ReturnCode = int;

    using ConfigFile = TmpFile;

    static czstring<> EXEC_HELPER_BINARY="exec-helper";
    static czstring<> COMMAND_KEY = "commands";
    static czstring<> COMMAND_LINE_COMMAND_KEY = "command-line-command";
    static czstring<> COMMAND_LINE_COMMAND_LINE_KEY = "command-line";

    const ReturnCode SUCCESS = EXIT_SUCCESS;

    void childProcessExecute(const CommandLineRaw& line, const string& workingDir) noexcept {
        ReturnCode returnCode;
        
        error_code error;
        current_path(workingDir, error);
        expectsMessage(error == success, "Either works or fix the test");
        
        // Suppress output
        int fd = open("/dev/null", O_WRONLY | O_CLOEXEC);
        expectsMessage(fd >= 0, "Either works or fix the test");
        returnCode = dup2(fd, 1);
        expectsMessage(returnCode >= 0, "Either works or fix the test");
        close(fd);

        if ((returnCode = execvp(line[0], &(line[0]))) == -1) {
            cout << "Could not execvp command: " << strerror(errno) << " (" << errno << ")" << endl; 
        }
        // execvp replaces the process: the return code for valid execvp calls should be obtained from the parent
        expectsMessage(false, "Either works or fix the test");
    }

    ReturnCode waitForChild(pid_t pid) noexcept {
        pid_t ret;
        ReturnCode status;
        while ((ret = waitpid(pid, &status, 0)) == -1) {
            if (errno != EINTR) {
                break;
            }
        }
        expectsMessage(ret != -1, "Either works or fix the test");
        if(status == 0) {
            return 0;
        }
        if (WIFEXITED(status) && WEXITSTATUS(status)) {
            return WEXITSTATUS(status);
        }
        return status;
    }

    int execute(const CommandLineRaw& commandLine, const string& workingDir = ".") {
        pid_t pid = fork();
        REQUIRE(pid != -1);
        if(pid == 0) {
            childProcessExecute(commandLine, workingDir);
        }
        return waitForChild(pid);
    }

    int execute(const CommandLine& commandLine, const string& workingDir = ".") {
        CommandLineRaw rawLine;
        for(const auto& arg : commandLine) {
            rawLine.push_back(strdup(arg.c_str()));
        }
        rawLine.push_back(nullptr);
        return execute(rawLine, workingDir);
    }

} // namespace 

namespace execHelper {
namespace integration {
    /**
     * Implements @ref execute-multiple-statements-predefined-order
     */
    SCENARIO("Execution order of a command consisting of one predefined statement", "[execute-multiple-statements][execute-multiple-statements-predefined-order][scenario-execute-multiple-statements-predefined-order-single]") {
        GIVEN("A valid configuration with multiple commands consisting of one predefined, valid, successful statement") {
            const Commands commands({"command1", "command2", "command3", "command4"});
            YamlWriter yaml;
            ExecutionHandler executions;

            for(const auto& command : commands) {
                executions.add(command, ExecutionContent());

                yaml[COMMAND_KEY][command] = "Execute the command";
                yaml[command] = COMMAND_LINE_COMMAND_KEY;
                yaml[COMMAND_LINE_COMMAND_KEY][command][COMMAND_LINE_COMMAND_LINE_KEY] = executions.at(command).getConfigCommand();
            }

            ConfigFileWriter config;
            config.write(yaml);

            WHEN("we call each of these commands separately") {
                for(const auto& command : commands) {
                    auto executionIterator = executions.startIteration();
                    const ReturnCode returnCode = execute({EXEC_HELPER_BINARY, command, "--settings-file", config.getFilename()}, config.getDirectory());

                    THEN_CHECK("the call for " + command + " should succeed") {
                        REQUIRE(returnCode == SUCCESS);
                    }

                    THEN_CHECK("the associated predefined statement for " + command + " should be called exactly once") {
                        REQUIRE(executions.at(command).getNumberOfExecutions() == 1U);
                    }

                    THEN("no other predefined statements except for" + command + " should have been called") {
                        for(const auto& otherCommand : commands) {
                            if(command != otherCommand) {
                                REQUIRE(executions.at(otherCommand).getNumberOfExecutions() == 0U);
                            }
                        }
                    }
                }
            }
        } 
    }

    /**
     * Implements @ref scenario-execute-multiple-statements-predefined-order-multiple
     */
    SCENARIO("Scenario: Execution order of commands consisting of multiple predefined statements", "[execute-multiple-statements][execute-multiple-statements-predefined-order][scenario-execute-multiple-statements-predefined-order-multiple]") {
        GIVEN("A valid configuration with multiple commands consisting of multiple, predefined, valid, successful statements") {
            const Commands commands({"command1", "command2", "command3", "command4"});
            YamlWriter yaml;
            ExecutionHandler executions;

            const unsigned int NB_OF_SUBCOMMANDS = 3U;

            for(const auto& command : commands) {
                executions.add(command, ExecutionContent());

                Commands subcommands;
                for(unsigned int i = 0; i < NB_OF_SUBCOMMANDS; ++i) {
                    string subcommand = command;
                    subcommand.append("-subcommand").append(to_string(i));
                    subcommands.emplace_back(subcommand);
                }

                yaml[COMMAND_KEY][command] = "Execute the command";
                yaml[command] = subcommands;

                for(const auto& subcommand : subcommands) {
                    yaml[subcommand] = COMMAND_LINE_COMMAND_KEY;
                    yaml[COMMAND_LINE_COMMAND_KEY][subcommand][COMMAND_LINE_COMMAND_LINE_KEY] = executions.at(command).getConfigCommand();
                }
            }

            ConfigFileWriter config;
            config.write(yaml);

            WHEN("we call each of these commands separately") {
                for(const auto& command : commands) {
                    auto iterator = executions.startIteration();
                    const ReturnCode returnCode = execute({EXEC_HELPER_BINARY, command, "--settings-file", config.getFilename()}, config.getDirectory());

                    THEN("the call with " + command + " should succeed") {
                        REQUIRE(returnCode == SUCCESS);
                    }

                    THEN("the associated predefined statements for " + command + " should be called the expected number of times in the right order") {
                        REQUIRE(executions.at(command).getNumberOfExecutions() == NB_OF_SUBCOMMANDS);
                    }

                    THEN("no other predefined statements except for " + command + " should have been called") {
                        for(const auto& otherCommand : commands) {
                            if(command != otherCommand) {
                                REQUIRE(executions.at(otherCommand).getNumberOfExecutions() == 0U);
                            }
                        }
                    }
                }
            }
        }
    }
} // namespace integration
} // namespace execHelper
