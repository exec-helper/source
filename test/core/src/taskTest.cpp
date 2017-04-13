#include <string>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <catch.hpp>

#include "utils/utils.h"
#include "core/task.h"

using std::string;
using std::istringstream;
using std::stringstream;
using std::vector;
using std::move;
using boost::split;

using execHelper::test::utils::appendVectors;

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("Check the tasks", "[task]") {
            GIVEN("A task and a line of commands") {
                Task task;
                string actualCommand = "command1 command2 command3";

                // Convert the string command to a collection of separate commands
                TaskCollection actualCommandCollection;
                split(actualCommandCollection, actualCommand, boost::is_any_of(" "));

                WHEN("We add each command separately to the task") {
                    for(const auto& command : actualCommandCollection) {
                        task.append(command);
                    }

                    THEN("We should be able to find back all commands in order") {
                        REQUIRE(task.getTask() == actualCommandCollection); 
                    }
                    THEN("We should get the accumulated string") {
                        REQUIRE(task.toString() == actualCommand); 
                    }
                }

                WHEN("We add the entire string as one single command parameter") {
                    task.append(actualCommand);

                    THEN("We should find it back as a single command parameter") {
                        TaskCollection resultTask = task.getTask();
                        REQUIRE(resultTask.size() == 1U);
                        REQUIRE(resultTask.front() == actualCommand); 
                    }
                    THEN("We should get the accumulated string") {
                        REQUIRE(task.toString() == actualCommand); 
                    }
                }

                WHEN("We add each command as a collection to the task") {
                    task.append(actualCommandCollection);

                    THEN("We should be able to find back all commands in order") {
                        REQUIRE(task.getTask() == actualCommandCollection); 
                    }
                    THEN("We should get the accumulated string") {
                        REQUIRE(task.toString() == actualCommand); 
                    }
                }
            }

            GIVEN("A task and an empty line of commands") {
                Task task;
                string actualCommand;

                // Convert the string command to a collection of separate commands
                TaskCollection actualCommandCollection;
                split(actualCommandCollection, actualCommand, boost::is_any_of(" "));

                WHEN("We add the entire string as one single command parameter") {
                    task.append(actualCommand);

                    THEN("We should find it back as a single command parameter") {
                        TaskCollection resultTask = task.getTask();
                        REQUIRE(resultTask.size() == 1U);
                        REQUIRE(resultTask.front() == actualCommand); 
                    }
                    THEN("We should get the accumulated string") {
                        REQUIRE(task.toString() == actualCommand); 
                    }
                }

                WHEN("We add each command as a collection to the task") {
                    task.append(actualCommandCollection);

                    THEN("We should be able to find back all commands in order") {
                        REQUIRE(task.getTask() == actualCommandCollection); 
                    }
                    THEN("We should get the accumulated string") {
                        REQUIRE(task.toString() == actualCommand); 
                    }
                }
            }

            GIVEN("A task and an empty line of commands") {
                Task task;
                string actualCommand;

                // Convert the string command to a collection of separate commands
                TaskCollection actualCommandCollection;
                split(actualCommandCollection, actualCommand, boost::is_any_of(" "));

                WHEN("We add the entire string as one single command parameter") {
                    task.append(actualCommand);

                    THEN("We should find it the empty string back as a single command parameter") {
                        TaskCollection resultTask = task.getTask();
                        REQUIRE(resultTask.size() == 1U);
                        REQUIRE(resultTask.front() == actualCommand); 
                    }
                    THEN("We should get the empty string") {
                        REQUIRE(task.toString() == actualCommand); 
                    }
                }

                WHEN("We add each command as a collection to the task") {
                    task.append(actualCommandCollection);

                    THEN("We should be able to find back the empty string in a collection") {
                        REQUIRE(task.getTask() == actualCommandCollection); 
                    }
                    THEN("We should get the empty string") {
                        REQUIRE(task.toString() == actualCommand); 
                    }
                }
            }
            GIVEN("A task and a line of commands") {
                Task task;
                string actualCommand = "command1 command2 command3";

                // Convert the string command to a collection of separate commands
                TaskCollection actualCommandCollection;
                split(actualCommandCollection, actualCommand, boost::is_any_of(" "));
                TaskCollection moveCommandCollection = actualCommandCollection;

                WHEN("We move each command separately to the task") {
                    for(const auto& command : moveCommandCollection) {
                        task.append(command);
                    }

                    THEN("We should be able to find back all commands in order") {
                        REQUIRE(task.getTask() == actualCommandCollection); 
                    }
                    THEN("We should get the accumulated string") {
                        REQUIRE(task.toString() == actualCommand); 
                    }
                }
                WHEN("We move all command together to the task") {
                    task.append(move(moveCommandCollection));

                    THEN("We should be able to find back all commands in order") {
                        REQUIRE(task.getTask() == actualCommandCollection); 
                    }
                    THEN("We should get the accumulated string") {
                        REQUIRE(task.toString() == actualCommand); 
                    }
                }
            }
            GIVEN("A task and an initializer list that initializes it") {
                Task task({"task1", "task2", "task3"});
                vector<string> additionalTasks({"taskA", "taskB", "taskC"});
                TaskCollection additionalCollection({"taskAlpha", "taskBeta"});

                WHEN("We add the additional tasks") {
                    for(const auto& subtask : additionalTasks) {
                        task.append(subtask);
                    } 
                    task.append(additionalCollection);

                    THEN("We should find all tasks in order") {
                        TaskCollection correctResults({"task1", "task2", "task3"});
                        appendVectors(correctResults, additionalTasks);
                        appendVectors(correctResults, additionalCollection);
                        REQUIRE(task.getTask() == correctResults);
                    }
                }
            }
        }

        SCENARIO("Check the task comparison operators", "[task]") {
            GIVEN("One task") {
                Task actualTask;
                actualTask.append("task1");
                actualTask.append("task2");

                WHEN("We compare it with itself") {
                    THEN("It should be true") {
                        REQUIRE(actualTask == actualTask);
                        REQUIRE_FALSE(actualTask != actualTask);
                    }
                }
            }
            GIVEN("Two matching tasks") {
                Task task1;
                task1.append("task1");
                task1.append("task2");

                Task task2;
                task2.append("task1");
                task2.append("task2");

                WHEN("We compare them") {
                    THEN("It should return true") {
                        REQUIRE(task1 == task2);
                        REQUIRE_FALSE(task1 != task2);
                    }
                }
            }
            GIVEN("Two non-matching tasks") {
                Task task1;
                task1.append("task1");
                task1.append("task2");

                Task task2;
                task2.append("task2");
                task2.append("task1");

                WHEN("We compare them") {
                    THEN("It should return false") {
                        REQUIRE_FALSE(task1 == task2);
                        REQUIRE(task1 != task2);
                    }
                }
            }
        }
    }
} }
