#include <catch.hpp>

#include <string>
#include <fstream>

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
                task.append("cat");
                task.append(filename);

                THEN("The call should succeed") {
                    REQUIRE(shell.execute(task) == 0);
                    REQUIRE(shell.isExecutedSuccessfully(shell.execute(task)));
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
                task.append("cat");
                task.append(filename);

                THEN("The call should not succeed") {
                    REQUIRE(shell.execute(task) != 0);
                    REQUIRE(shell.isExecutedSuccessfully(shell.execute(task)) == false);
                }
            }
        }
    }

} } }
