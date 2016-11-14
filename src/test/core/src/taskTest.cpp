#include <vector>
#include <string>
#include <sstream>

#include <catch.hpp>

#include "core/task.h"

using std::string;
using std::vector;
using std::istringstream;
using std::stringstream;
using std::istream_iterator;

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
        }
    }
} }
