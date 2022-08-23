#include <memory>
#include <string>

#include "core/immediateExecutor.h"
#include "unittest/catch.h"

#include "shellStub.h"

using std::string;

namespace execHelper::core::test {
SCENARIO("Test the execution of the immediateExecutor",
         "[ExecutorInterface][ImmediateExecutor]") {
    GIVEN("Some tasks we want to execute and an executor") {
        Task task1;
        task1.append("task1");
        Task task2;
        task2.append("task2");
        Task task3;
        task3.append("task3");

        ShellStub::TaskQueue actualTasks = {task1, task2, task3};
        ShellStub shell;
        ImmediateExecutor executor(
            shell, []([[maybe_unused]] Shell::ShellReturnCode returnCode) {});

        WHEN("We schedule each task and run the executor") {
            executor.execute(task1);
            executor.execute(task2);
            executor.execute(task3);

            THEN("We should get the same tasks again") {
                const ShellStub::TaskQueue& executedTasks =
                    shell.getExecutedTasks();
                REQUIRE(executedTasks == actualTasks);
            }
        }
    }
}

SCENARIO("Test the failing of the execution",
         "[ExecutorInterface][ImmediateExecutor]") {
    GIVEN("A shell that fails to execute and a task to execute") {
        Task task1;
        task1.append("task1");
        const Shell::ShellReturnCode actualReturnCode = 42U;
        Shell::ShellReturnCode realReturnCode = 0U;

        ImmediateExecutor::Callback callback =
            [&realReturnCode](Shell::ShellReturnCode returnCode) {
                realReturnCode = returnCode;
            };
        ShellStub shell(actualReturnCode);
        ImmediateExecutor executor(shell, callback);

        WHEN("We schedule the task for execution") {
            executor.execute(task1);

            THEN("We should receive the failed return code") {
                REQUIRE(realReturnCode == actualReturnCode);
            }
        }
    }
}
} // namespace execHelper::core::test
