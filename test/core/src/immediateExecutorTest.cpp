#include <memory>
#include <string>

#include "core/immediateExecutor.h"
#include "unittest/catch.h"

#include "shellStub.h"

using std::make_shared;
using std::static_pointer_cast;
using std::string;

using execHelper::core::ImmediateExecutor;
using execHelper::core::Shell;

namespace {
ImmediateExecutor::Callback // NOLINT(fuchsia-statically-constructed-objects)
    IGNORE_CALLBACK = [](
        Shell::
            ShellReturnCode /* returnCode */) noexcept {}; // NOLINT(cert-err58-cpp)
} // namespace

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
        auto shell = make_shared<ShellStub>();
        ImmediateExecutor executor(static_pointer_cast<Shell>(shell),
                                   IGNORE_CALLBACK);

        WHEN("We schedule each task and run the executor") {
            executor.execute(task1);
            executor.execute(task2);
            executor.execute(task3);

            THEN("We should get the same tasks again") {
                ShellStub::TaskQueue executedTasks = shell->getExecutedTasks();
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
        auto shell = make_shared<ShellStub>(actualReturnCode);
        ImmediateExecutor executor(static_pointer_cast<Shell>(shell), callback);

        WHEN("We schedule the task for execution") {
            executor.execute(task1);

            THEN("We should receive the failed return code") {
                REQUIRE(realReturnCode == actualReturnCode);
            }
        }
    }
}
} // namespace execHelper::core::test
