#include <cstdio>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <sys/stat.h>

#include <boost/lexical_cast.hpp>
#include <boost/process/search_path.hpp>

#include "base-utils/executionContent.h"
#include "base-utils/generators.h"
#include "base-utils/tmpFile.h"
#include "config/path.h"
#include "core/posixShell.h"
#include "core/task.h"
#include "unittest/catch.h"
#include "unittest/rapidcheck.h"
#include "utils/utils.h"

using std::ofstream;
using std::optional;
using std::string;

using boost::lexical_cast;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Path;

using execHelper::test::propertyTest;
using execHelper::test::baseUtils::ExecutionContent;
using execHelper::test::baseUtils::IoService;
using execHelper::test::baseUtils::TmpFile;

namespace filesystem = std::filesystem;
namespace process = boost::process;
namespace this_process = boost::this_process;

namespace {
/**
 * Searches for the given binary file in the current directory and the given path
 *
 * @param[in]   binary  The binary to search for
 * @param[in]   paths   The current paths to search in
 * @returns The full path to the binary
 */
optional<filesystem::path> searchBinary(
    const std::string& binary,
    std::vector<boost::filesystem::path> paths = this_process::path()) {
    paths.emplace_back(".");

    auto result = process::search_path(binary, paths).native();
    if(result.empty()) {
        return std::nullopt;
    }
    return result;
}
} // namespace

namespace execHelper::core::test {
SCENARIO("Test that the right return code is returned", "[shell][posixshell]") {
    propertyTest("An execution engine and a shell",
                 [](PosixShell::ShellReturnCode expectedReturnCode) {
                     IoService ioService;

                     PosixShell shell;

                     ExecutionContent::registerIoService(
                         gsl::not_null<IoService*>(&ioService));
                     ExecutionContent executionEngine(expectedReturnCode);

                     ioService.run();

                     WHEN("We execute the command of the execution engine") {
                         Task task;
                         task.append(executionEngine.getConfigCommand());
                         auto actualReturnCode = shell.execute(task);

                         THEN_CHECK("The actual return code should equal the "
                                    "expected return code") {
                             REQUIRE(actualReturnCode == expectedReturnCode);
                         }
                     }
                 });
}

SCENARIO("Test that the command is executed the expected number of times",
         "[shell][posixshell]") {
    propertyTest("An execution engine and a shell", [](uint8_t nbOfRepeats) {
        IoService ioService;

        PosixShell shell;

        ExecutionContent::registerIoService(
            gsl::not_null<IoService*>(&ioService));
        ExecutionContent executionEngine(0);

        ioService.run();

        THEN_WHEN("We execute the command of the execution engine the expected "
                  "number of times") {
            for(uint8_t i = 0; i < nbOfRepeats; ++i) {
                Task task;
                task.append(executionEngine.getConfigCommand());
                shell.execute(task);
            }

            THEN_CHECK("We should find the expected number of executions") {
                REQUIRE(executionEngine.getNumberOfExecutions() == nbOfRepeats);
            }

            THEN_CHECK(
                "We should find the expected number of empty data members") {
                const auto& data = executionEngine.getReceivedData();

                REQUIRE(data.size() == nbOfRepeats);

                for(const auto& entry : data) {
                    REQUIRE(entry.args.empty());
                }
            }
        }
    });
}

SCENARIO("Test non-existing binaries", "[shell][posixshell]") {
    propertyTest("A non-existing file and a shell", [](const TmpFile& file) {
        PosixShell shell;

        THEN_WHEN("We try to execute the given file") {
            Task task({file.toString()});

            THEN_CHECK("An exception should be thrown during execution") {
                REQUIRE_THROWS_AS(shell.execute(task),
                                  execHelper::core::PathNotFoundError);
            }
        }
    });
}

SCENARIO("Test the shell for shell expansion", "[shell][posixshell]") {
    propertyTest(
        "An execution engine and a shell", [](const uint32_t randomExpansion) {
            IoService ioService;

            PosixShell shell;

            ExecutionContent::registerIoService(
                gsl::not_null<IoService*>(&ioService));
            ExecutionContent executionEngine(0);

            ioService.run();

            THEN_WHEN("We execute the command using shell expansion") {
                Task task;
                task.append(executionEngine.getConfigCommand());
                task.append("\"$(echo " + std::to_string(randomExpansion) +
                            ")\"");
                shell.execute(task);

                THEN_CHECK("The execution engine should have received the "
                           "expanded value") {
                    auto data = executionEngine.getReceivedData();

                    REQUIRE(data.size() == 1U);
                    REQUIRE(data.front().args.size() == 1U);
                    const auto receivedArgument =
                        lexical_cast<uint32_t>(data.front().args.front());
                    REQUIRE(receivedArgument == randomExpansion);
                }
            }
        });
}

SCENARIO("Test the shell for word expansion", "[shell][posixshell]") {
    propertyTest(
        "An execution engine and a shell",
        [](const std::string& shellExpansion) {
            RC_PRE(!shellExpansion.empty());
            IoService ioService;

            EnvironmentCollection env = {{"EXPANSION", shellExpansion}};
            PosixShell shell;

            ExecutionContent::registerIoService(
                gsl::not_null<IoService*>(&ioService));
            ExecutionContent executionEngine(0);

            ioService.run();

            THEN_WHEN("We execute the command using word expansion") {
                Task task;
                task.setEnvironment(env);
                task.append(executionEngine.getConfigCommand());
                task.append("\"${EXPANSION}\"");
                shell.execute(task);

                THEN_CHECK("The execution engine should have received the "
                           "expanded value") {
                    auto data = executionEngine.getReceivedData();

                    REQUIRE(data.size() == 1U);
                    REQUIRE(data.front().args.size() == 1U);
                    REQUIRE(data.front().args.front() == shellExpansion);
                }
            }
        });
}

SCENARIO("Test the shell for binaries prefixed with an absolute path",
         "[shell][posixshell]") {
    propertyTest("An execution engine and a shell", []() {
        IoService ioService;

        PosixShell shell;

        ExecutionContent::registerIoService(
            gsl::not_null<IoService*>(&ioService));
        ExecutionContent executionEngine(0);

        ioService.run();

        THEN_WHEN("We replace the binary to execute with its absolute path") {
            // Add the current directory to the path
            //auto path = this_process::path();
            //path.emplace_back(".");

            auto command = executionEngine.getConfigCommand();
            auto binary = searchBinary(command[0]);
            REQUIRE(binary);

            command[0] = binary->string();

            Task task;
            task.append(command);
            auto actualReturnCode = shell.execute(task);

            THEN_CHECK("The actual return code should equal the expected "
                       "return code") {
                REQUIRE(shell.isExecutedSuccessfully(actualReturnCode));
            }

            THEN_CHECK("The command should be properly executed") {
                REQUIRE(executionEngine.getNumberOfExecutions() == 1U);
            }
        }
    });
}

SCENARIO("Test the shell for binaries prefixed with a relative path",
         "[shell][posixshell]") {
    propertyTest("An execution engine and a shell", []() {
        IoService ioService;

        PosixShell shell;

        ExecutionContent::registerIoService(
            gsl::not_null<IoService*>(&ioService));
        ExecutionContent executionEngine(0);

        ioService.run();

        THEN_WHEN("We replace the binary to execute with its relative path") {
            // Add the current directory to the path
            //auto path = this_process::path();
            //path.emplace_back(".");

            auto command = executionEngine.getConfigCommand();
            auto binary = searchBinary(command[0]);
            REQUIRE(binary);

            command[0] =
                filesystem::relative(*binary, filesystem::current_path())
                    .native();

            Task task;
            task.append(command);
            shell.execute(task);

            THEN_CHECK("The command should be properly executed") {
                REQUIRE(executionEngine.getNumberOfExecutions() == 1U);
            }
        }
    });
}

SCENARIO("Test the shell for binaries found in the task environment path",
         "[shell][posixshell]") {
    propertyTest("An execution engine and a shell", []() {
        IoService ioService;

        PosixShell shell;

        ExecutionContent::registerIoService(
            gsl::not_null<IoService*>(&ioService));
        ExecutionContent executionEngine(0);

        ioService.run();

        THEN_WHEN("We add the absolute path of the binary as the one entry of "
                  "the PATH environment variable") {
            auto command = executionEngine.getConfigCommand();
            auto binary = searchBinary(command[0]);
            REQUIRE(binary);

            auto newPath = *binary;
            newPath.remove_filename();

            REQUIRE(
                newPath !=
                newPath
                    .parent_path()); // The test will not test this property properly if the parent path equals the current path

            EnvironmentCollection env = {{"PATH", newPath.native()}};

            Task task;
            task.append(command);
            task.setEnvironment(env);
            task.setWorkingDirectory(newPath.parent_path().native());
            shell.execute(task);

            THEN_CHECK("The command should be properly executed") {
                REQUIRE(executionEngine.getNumberOfExecutions() == 1U);
            }
        }
    });
}

SCENARIO("Test the shell for binaries found in the working directory but not "
         "in the path",
         "[shell][posixshell]") {
    propertyTest("An execution engine and a shell", []() {
        IoService ioService;

        PosixShell shell;

        ExecutionContent::registerIoService(
            gsl::not_null<IoService*>(&ioService));
        ExecutionContent executionEngine(0);

        ioService.run();

        THEN_WHEN("We add the absolute path of the binary as the one entry of "
                  "the PATH environment variable") {
            auto command = executionEngine.getConfigCommand();
            auto binary = searchBinary(command[0]);
            REQUIRE(binary);

            auto workingDir =
                binary
                    ->parent_path(); // The binary will be found in the working directory

            EnvironmentCollection env = {
                {"PATH",
                 ""}}; // Make sure that the PATH is empty, so it will not be found there

            Task task;
            task.append(command);
            task.setEnvironment(env);
            task.setWorkingDirectory(workingDir);
            shell.execute(task);

            THEN_CHECK("The command should be properly executed") {
                REQUIRE(executionEngine.getNumberOfExecutions() == 1U);
            }
        }
    });
}
} // namespace execHelper::core::test
