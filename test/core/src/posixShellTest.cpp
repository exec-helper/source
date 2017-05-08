#include <cstdio>
#include <fstream>
#include <string>

#include <catch.hpp>

#include "core/posixShell.h"
#include "core/task.h"
#include "utils/utils.h"

using std::ofstream;
using std::string;
using std::remove;

using execHelper::test::utils::fileExists;

namespace execHelper { namespace core { namespace test {
    SCENARIO("Test the posix shell for successfull commands", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            const string filename("test-posix-shell.exec-helper");
            remove(filename.c_str());   // Make sure the file does not exist beforehand

            PosixShell shell;

            WHEN("We apply it on a task to show the file") {
                Task task;
                task.append("touch");
                task.append(filename);

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(fileExists(filename));
                }
            }
            remove(filename.c_str());
        }
    }

    SCENARIO("Test the posix shell for shell expansion", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            const string filename("test-posix-shell.exec-helper");
            remove(filename.c_str());   // Make sure the file does not exist beforehand

            PosixShell shell;

            WHEN("We apply it on a task to show the file") {
                Task task;
                task.append("touch");
                task.append("$(pwd)/" + filename);

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(fileExists(filename));
                }
            }
            remove(filename.c_str());
        }
    }

    SCENARIO("Test the posix shell for word expansion", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            const string filename("test-posix-shell.exec-helper");
            remove(filename.c_str());   // Make sure the file does not exist beforehand

            EnvironmentCollection env = {{"PWD", "."}};

            PosixShell shell;

            WHEN("We use a command with variable expansion") {
                Task task(env);
                task.append("touch");
                task.append("$PWD/" + filename);

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(fileExists(filename));
                }
            }

            WHEN("We use a command with variable expansion") {
                Task task(env);
                task.append("touch");
                task.append("${PWD}/" + filename);

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(fileExists(filename));
                }
            }

            WHEN("We use a command with command expansion") {
                Task task;
                task.append("touch");
                task.append("$(pwd)/" + filename);

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(fileExists(filename));
                }
            }

            WHEN("We use a command with variable not inherited from the outside environment") {
                env.emplace(make_pair("SELF_DEFINED_FILENAME", filename));

                Task task(env);
                task.append("touch");
                task.append("./${SELF_DEFINED_FILENAME}");

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(fileExists(filename));
                }
            }
            remove(filename.c_str());
        }
    }

    SCENARIO("Test the posix shell for unsuccessfull commands", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            string filename("non-existing-file.file");
            PosixShell shell;

            WHEN("We apply it on a task to show the file") {
                Task task;
                task.append("cat");
                task.append(filename);

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should not succeed") {
                    REQUIRE(returnCode != 0);
                    REQUIRE_FALSE(shell.isExecutedSuccessfully(returnCode));
                }
            }
        }
    }
} } }
