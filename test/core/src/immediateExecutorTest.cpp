#include <string>

#include <catch.hpp>

#include "core/immediateExecutor.h"
#include "shellStub.h"

using std::string;

using execHelper::core::ImmediateExecutor;
using execHelper::core::Shell;

namespace {
    ImmediateExecutor::Callback IGNORE_CALLBACK = [](Shell::ShellReturnCode /* returnCode */) {};
}

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("Test the execution of the immediateExecutor", "[ExecutorInterface][ImmediateExecutor]") {
            GIVEN("Some tasks we want to execute and an executor") {
                Task task1;
                task1.append("task1");
                Task task2;
                task2.append("task2");
                Task task3;
                task3.append("task3");

                ShellStub::TaskQueue actualTasks = {task1, task2, task3};
                ShellStub shell;
                ImmediateExecutor executor(shell, IGNORE_CALLBACK);

                WHEN("We schedule each task and run the executor") {
                    REQUIRE(executor.execute(task1));
                    REQUIRE(executor.execute(task2));
                    REQUIRE(executor.execute(task3));

                    THEN("We should get the same tasks again") {
                        ShellStub::TaskQueue executedTasks = shell.getExecutedTasks();
                        REQUIRE(executedTasks == actualTasks);
                    }
                }
            }
        }

        SCENARIO("Test the failing of the execution", "[ExecutorInterface][ImmediateExecutor]") {
            GIVEN("A shell that fails to execute and a task to execute") {
                Task task1;
                task1.append("task1");
                const Shell::ShellReturnCode actualReturnCode = 42U;
                Shell::ShellReturnCode realReturnCode = 0U;

                ShellStub shell(actualReturnCode);
                ImmediateExecutor::Callback callback = [&realReturnCode](Shell::ShellReturnCode returnCode) { realReturnCode = returnCode; };
                ImmediateExecutor executor(shell, callback);

                WHEN("We schedule the task for execution") {
                    bool returnCode = executor.execute(task1);

                    THEN("The call should fail") {
                        REQUIRE_FALSE(returnCode);
                    }
                    THEN("We should receive the failed return code") {
                        REQUIRE(realReturnCode == actualReturnCode);
                    }
                }
            }
        }
    }
} }
