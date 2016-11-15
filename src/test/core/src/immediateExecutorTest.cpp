#include <string>

#include <catch.hpp>

#include "core/immediateExecutor.h"
#include "shellStub.h"

using std::string;

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
                ImmediateExecutor executor(shell);

                WHEN("We schedule each task and run the executor") {
                    executor.execute(task1);
                    executor.execute(task2);
                    executor.execute(task3);

                    THEN("We should get the same tasks again") {
                        ShellStub::TaskQueue executedTasks = shell.getExecutedTasks();
                        REQUIRE(executedTasks == actualTasks);
                    }
                }
            }
        }
    }
} }
