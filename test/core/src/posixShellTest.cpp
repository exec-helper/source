#include <fstream>
#include <string>

#include <catch.hpp>

#include "core/posixShell.h"
#include "core/task.h"

using std::ofstream;
using std::string;
using std::endl;

namespace execHelper { namespace core { namespace test {
    SCENARIO("Test the posix shell for successfull commands", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            string filename("test-posix-shell.exec-helper");
            string fileContent("Hello world!");
            ofstream file;
            file.open(filename);
            file << fileContent << endl;
            file.close();

            PosixShell shell;

            WHEN("We apply it on a task to show the file") {
                Task task;
                task.append("/usr/bin/cat");
                task.append(filename);

                THEN("The call should succeed") {
                    PosixShell::ShellReturnCode returnCode = shell.execute(task);
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }
            }
        }
    }

    SCENARIO("Test the environment variable", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            string filename("test-posix-shell.exec-helper");
            string fileContent("Hello world!");
            ofstream file;
            file.open(filename);
            file << fileContent << endl;
            file.close();

            EnvironmentCollection env = {{"PATH", "/usr/bin"}};

            PosixShell shell;

            WHEN("We apply it on a task to show the file") {
                Task task(env);
                task.append("cat");
                task.append(filename);

                THEN("The call should succeed") {
                    PosixShell::ShellReturnCode returnCode = shell.execute(task);
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }
            }
        }
    }

    SCENARIO("Test the posix shell for shell expansion", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            string filename("test-posix-shell.exec-helper");
            const string fileContent("Hello world!");
            ofstream file;
            file.open(filename);
            file << fileContent << endl;
            file.close();

            string filename2("test-posix-shell2.exec-helper");
            file.open(filename2);
            file << fileContent << endl;
            file.close();

            PosixShell shell;

            WHEN("We apply it on a task to show the file") {
                Task task;
                task.append("/usr/bin/cat");
                task.append("*.exec-helper");

                THEN("The call should succeed") {
                    PosixShell::ShellReturnCode returnCode = shell.execute(task);
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }
            }
        }
    }

    SCENARIO("Test the posix shell for word expansion", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            string filename("test-posix-shell.exec-helper");
            const string fileContent("Hello world!");
            ofstream file;
            file.open(filename);
            file << fileContent << endl;
            file.close();

            EnvironmentCollection env = {{"PWD", "."}};

            PosixShell shell;

            WHEN("We use a command with variable expansion") {
                Task task(env);
                task.append("/usr/bin/cat");
                task.append("$PWD/" + filename);

                THEN("The call should succeed") {
                    PosixShell::ShellReturnCode returnCode = shell.execute(task);
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }
            }

            WHEN("We use a command with variable expansion") {
                Task task(env);
                task.append("/usr/bin/cat");
                task.append("${PWD}/" + filename);

                THEN("The call should succeed") {
                    PosixShell::ShellReturnCode returnCode = shell.execute(task);
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }
            }

            WHEN("We use a command with command expansion") {
                Task task;
                task.append("/usr/bin/cat");
                task.append("$(pwd)/" + filename);

                THEN("The call should succeed") {
                    PosixShell::ShellReturnCode returnCode = shell.execute(task);
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }
            }
        }
    }

    SCENARIO("Test the posix shell for unsuccessfull commands", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            string filename("non-existing-file.file");
            PosixShell shell;

            WHEN("We apply it on a task to show the file") {
                Task task;
                task.append("/usr/bin/cat");
                task.append(filename);

                THEN("The call should not succeed") {
                    REQUIRE(shell.execute(task) != 0);
                    REQUIRE_FALSE(shell.isExecutedSuccessfully(shell.execute(task)));
                }
            }
        }
    }

} } }
