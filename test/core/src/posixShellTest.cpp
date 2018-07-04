#include <cstdio>
#include <string>

#include <boost/filesystem/operations.hpp>

#include "base-utils/tmpFile.h"
#include "config/path.h"
#include "core/posixShell.h"
#include "core/task.h"
#include "unittest/catch.h"
#include "utils/utils.h"

using std::string;

using execHelper::config::EnvironmentCollection;
using execHelper::config::Path;

using execHelper::test::baseUtils::TmpFile;

namespace execHelper {
namespace core {
namespace test {
SCENARIO("Test the posix shell for successfull commands",
         "[shell][posixshell]") {
    GIVEN("A posix shell and a file that can be shown in it") {
        const TmpFile file;

        PosixShell shell;

        WHEN("We apply it on a task to show the file") {
            Task task({"touch", file.getFilename()});
            task.setWorkingDirectory(file.getPath().parent_path());

            PosixShell::ShellReturnCode returnCode = shell.execute(task);

            THEN("The call should succeed") {
                REQUIRE(shell.isExecutedSuccessfully(returnCode));
            }

            THEN("The file should exist") { REQUIRE(file.exists()); }
        }
    }
}

SCENARIO("Test the posix shell for shell expansion", "[shell][posixshell]") {
    GIVEN("A posix shell and a file that can be shown in it") {
        const TmpFile file;

        PosixShell shell;

        WHEN("We apply it on a task to show the file") {
            Task task({"touch", "$(echo '" + file.getParentDirectory() + "')/" +
                                    file.getFilename()});
            PosixShell::ShellReturnCode returnCode = shell.execute(task);

            THEN("The call should succeed") {
                REQUIRE(shell.isExecutedSuccessfully(returnCode));
            }

            THEN("The file should exist") { REQUIRE(file.exists()); }
        }
    }
}

SCENARIO("Test the posix shell for word expansion", "[shell][posixshell]") {
    GIVEN("A posix shell and a file that can be shown in it") {
        TmpFile file;
        EnvironmentCollection env = {{"PWD", file.getParentDirectory()}};
        PosixShell shell;

        WHEN("We use a command with variable expansion") {
            Task task({"touch", "$PWD/" + file.getFilename()});
            task.setEnvironment(env);
            PosixShell::ShellReturnCode returnCode = shell.execute(task);

            THEN("The call should succeed") {
                REQUIRE(shell.isExecutedSuccessfully(returnCode));
            }

            THEN("The file should exist") { REQUIRE(file.exists()); }
        }

        WHEN("We use a command with variable expansion") {
            Task task({"touch", "${PWD}/" + file.getFilename()});
            task.setEnvironment(env);

            PosixShell::ShellReturnCode returnCode = shell.execute(task);

            THEN("The call should succeed") {
                REQUIRE(shell.isExecutedSuccessfully(returnCode));
            }

            THEN("The file should exist") { REQUIRE(file.exists()); }
        }

        WHEN("We use a command with variable not inherited from the outside "
             "environment") {
            env.emplace(make_pair("SELF_DEFINED_FILENAME", file.getFilename()));
            env.emplace(
                make_pair("SELF_DEFINED_DIRECTORY", file.getParentDirectory()));

            Task task({"touch",
                       "${SELF_DEFINED_DIRECTORY}/${SELF_DEFINED_FILENAME}"});
            task.setEnvironment(env);

            PosixShell::ShellReturnCode returnCode = shell.execute(task);

            THEN("The call should succeed") {
                REQUIRE(shell.isExecutedSuccessfully(returnCode));
            }

            THEN("The file should exist") { REQUIRE(file.exists()); }
        }
    }
}

SCENARIO("Test the posix shell for unsuccessfull commands",
         "[shell][posixshell]") {
    GIVEN("A posix shell and a file that can be shown in it") {
        TmpFile file;
        PosixShell shell;

        WHEN("We apply it on a task to show the file") {
            Task task({"cat", file.toString()});

            PosixShell::ShellReturnCode returnCode = shell.execute(task);

            THEN("The call should not succeed") {
                REQUIRE(returnCode != 0);
                REQUIRE_FALSE(shell.isExecutedSuccessfully(returnCode));
            }
        }
    }
}
} // namespace test
} // namespace core
} // namespace execHelper
