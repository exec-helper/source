#include "unittest/catch.h"

#include <fcntl.h>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include <boost/filesystem.hpp>
#include <gsl/string_span>

#include "base-utils/configFileWriter.h"
#include "base-utils/executionContent.h"
#include "base-utils/executionHandler.h"
#include "base-utils/tmpFile.h"
#include "log/assertions.h"

using std::cout;
using std::endl;
using std::mt19937;
using std::ofstream;
using std::string;
using std::stringstream;
using std::to_string;
using std::uniform_int_distribution;
using std::vector;

using boost::filesystem::current_path;
using boost::system::error_code;
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

    const czstring<> EXEC_HELPER_BINARY="exec-helper";
    const czstring<> COMMAND_KEY = "commands";
    const czstring<> COMMAND_LINE_COMMAND_KEY = "command-line-command";
    const czstring<> COMMAND_LINE_COMMAND_LINE_KEY = "command-line";

    const ReturnCode SUCCESS = EXIT_SUCCESS;

    void childProcessExecute(const CommandLineRaw& line, const string& workingDir) noexcept {
        ReturnCode returnCode;
        
        error_code error;
        current_path(workingDir, error);
        expectsMessage(error == boost::system::errc::success, "Either works or fix the test");
        
        int fd = ::open("/dev/null", O_WRONLY | O_CLOEXEC);     // NOLINT(cppcoreguidelines-pro-type-vararg)
        expectsMessage(fd >= 0, "Either works or fix the test");
        returnCode = dup2(fd, 1);
        if(returnCode == 0) {
            cout << "Error: could not duplicate stdout file descriptor in child process (" << returnCode << ")" << endl;
            expectsMessage(false, "Either works or fix the test");
        }
        close(fd);

        if ((returnCode = execvp(line[0], &(line[0]))) == -1) {
            cout << "Error (" << returnCode << "): Could not execvp command: " << strerror(errno) << " (" << errno << ")" << endl; 
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
        rawLine.reserve(commandLine.size());
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

    /**
     * Implements @ref scenario-execute-multiple-statements-predefined-order-duplicates
     */
    SCENARIO("Duplicate configured statements", "[execute-multiple-statements][scenario-execute-multiple-statements-predefined-order-duplicates]") {
        GIVEN("A valid configuration with a command consisting of multiple, duplicate statements and some non-duplicate ones") {
            const Commands commands({"command1", "command2", "command3", "command4"});
            YamlWriter yaml;
            ExecutionHandler executions;

            for(const auto& command : commands) {
                executions.add(command, ExecutionContent());

                yaml[COMMAND_KEY][command] = "Execute the command";
                yaml[command] = COMMAND_LINE_COMMAND_KEY;
                yaml[COMMAND_LINE_COMMAND_KEY][command][COMMAND_LINE_COMMAND_LINE_KEY] = executions.at(command).getConfigCommand();
            }

            const Command commandWithDuplicates("command-with-duplicates");
            yaml[COMMAND_KEY][commandWithDuplicates] = "Execute the command";

            const unsigned int NB_OF_DUPLICATE_STATEMENTS = 5U;
            const Command duplicateStatement("duplicate-statement");
            executions.add(commandWithDuplicates, ExecutionContent());
            yaml[COMMAND_LINE_COMMAND_KEY][duplicateStatement][COMMAND_LINE_COMMAND_LINE_KEY] = executions.at(commandWithDuplicates).getConfigCommand();
            yaml[duplicateStatement] = COMMAND_LINE_COMMAND_KEY;
            for(unsigned int i = 0U; i < NB_OF_DUPLICATE_STATEMENTS; ++i) {
                yaml[commandWithDuplicates].push_back(duplicateStatement);
            }

            const unsigned int NB_OF_UNIQUE_STATEMENTS = 5U;
            for(unsigned int i = 0U; i < NB_OF_UNIQUE_STATEMENTS; ++i) {
                Command uniqueStatement("unique-statement-" + to_string(i));

                yaml[uniqueStatement] = COMMAND_LINE_COMMAND_KEY;
                yaml[COMMAND_LINE_COMMAND_KEY][uniqueStatement][COMMAND_LINE_COMMAND_LINE_KEY] = executions.at(commandWithDuplicates).getConfigCommand();
                yaml[commandWithDuplicates].push_back(uniqueStatement);
            }

            ConfigFileWriter config;
            config.write(yaml);

            WHEN("we call this command") {
                auto iterator = executions.startIteration();
                const ReturnCode returnCode = execute({EXEC_HELPER_BINARY, commandWithDuplicates, "--settings-file", config.getFilename()}, config.getDirectory());

                THEN("the call should succeed") {
                    REQUIRE(returnCode == SUCCESS);
                }

                THEN("the associated predefined statements should all be called the expected number of times") {
                    REQUIRE(executions.at(commandWithDuplicates).getNumberOfExecutions() == NB_OF_DUPLICATE_STATEMENTS + NB_OF_UNIQUE_STATEMENTS);
                }

                THEN("no other predefined statements should have been called") {
                    for(const auto& command : commands) {
                        REQUIRE(executions.at(command).getNumberOfExecutions() == 0U);
                    }
                }
            }
        }
    }

    /**
     * Implements @ref scenario-execute-multiple-statements-predefined-order-failing-statements
     */
    SCENARIO("Execute multiple statements: predefined order: failing statements", "[execute-multiple-statements][scenario-execute-multiple-statements-predefined-order-failing-statements]") {
        GIVEN("A valid configuration with a command consisting of multiple, predefined, valid, successful statements and a failing statement that is not the first nor the last statement") {
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

                REQUIRE(subcommands.size() >= 3);   // In order to have a failing command that is not the first nor the last statement, a statement size of at least 3 statements is required

                // Define a failing command in a random way
                std::mt19937 gen(0);
                std::uniform_int_distribution<> dis(1, subcommands.size() - 2U);    // Find a random index that will fail, that is not the first nor the last one

                yaml[COMMAND_KEY][command] = "Execute the command";
                yaml[command] = subcommands;

                for(const auto& subcommand : subcommands) {
                    yaml[subcommand] = COMMAND_LINE_COMMAND_KEY;
                    yaml[COMMAND_LINE_COMMAND_KEY][subcommand][COMMAND_LINE_COMMAND_LINE_KEY] = executions.at(command).getConfigCommand();
                }
            }

            ConfigFileWriter config;
            config.write(yaml);


        }
    }
} // namespace integration
} // namespace execHelper
