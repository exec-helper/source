#include <string>
#include <sstream>

#include <catch.hpp>

#include "core/task.h"

using std::string;
using std::istringstream;
using std::stringstream;
using std::istream_iterator;
using std::move;

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("Check the tasks", "[task]") {
            GIVEN("A task and a line of commands") {
                Task task;
                string actualCommand = "command1 command2 command3";

                // Convert the string command to a collection of separate commands
                istringstream iss(actualCommand);
                TaskCollection actualCommandCollection{istream_iterator<string>{iss}, istream_iterator<string>{}};

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
                string actualCommand = "";

                // Convert the string command to a collection of separate commands
                istringstream iss(actualCommand);
                TaskCollection actualCommandCollection{istream_iterator<string>{iss}, istream_iterator<string>{}};

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
                string actualCommand = "";

                // Convert the string command to a collection of separate commands
                istringstream iss(actualCommand);
                TaskCollection actualCommandCollection{istream_iterator<string>{iss}, istream_iterator<string>{}};

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
            GIVEN("A task and a line of commands") {
                Task task;
                string actualCommand = "command1 command2 command3";

                // Convert the string command to a collection of separate commands
                istringstream iss(actualCommand);
                TaskCollection actualCommandCollection{istream_iterator<string>{iss}, istream_iterator<string>{}};
                TaskCollection moveCommandCollection = actualCommandCollection;

                WHEN("We move each command separately to the task") {
                    for(const auto& command : moveCommandCollection) {
                        task.append(move(command));
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
