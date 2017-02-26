#include <string>

#include <catch.hpp>

#include "core/sequentialTaskRunner.h"

#include "executorStub.h"

using std::string;

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("We want to schedule some tasks on the task runner", "[TaskRunner][SequentialTaskRunner]") {
            GIVEN("Some tasks we want to schedule and a scheduler") {
                Task task1;
                task1.append("task1");
                Task task2;
                task2.append("task2");
                Task task3;
                task3.append("task3");

                ExecutorStub::TaskQueue actualTasks = {task1, task2, task3};
                SequentialTaskRunner taskRunner;

                WHEN("We schedule each task and run the executor") {
                    taskRunner.addTask(task1);
                    taskRunner.addTask(task2);
                    taskRunner.addTask(task3);

                    ExecutorStub executor;
                    taskRunner.run(executor);

                    THEN("We should get the same tasks again") {
                        ExecutorStub::TaskQueue executedTasks = executor.getExecutedTasks();
                        REQUIRE(executedTasks == actualTasks);
                    }
                }
            }
        }
    }
} }
