#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "config/path.h"
#include "core/task.h"
#include "unittest/catch.h"
#include "utils/utils.h"

using std::endl;
using std::move;
using std::string;
using std::stringstream;
using std::vector;

using boost::filesystem::current_path;
using boost::trim_right;

using execHelper::config::Path;
using execHelper::core::TaskCollection;

using execHelper::test::utils::appendVectors;

namespace {
    template<typename T>
    inline vector<T> reverse(vector<T> toReverse) {
        std::reverse(toReverse.begin(), toReverse.end());
        return toReverse;
    }

    string toString(const TaskCollection& task) {
        string result;
        for(const auto& taskPart : task) {
            result.append(taskPart).append(" ");
        }
        trim_right(result);
        return result;
    }
} // namespace

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("Test the getters and setters of a task", "[task]") {
            GIVEN("A task") {
                MAKE_COMBINATIONS("Of getting and setting different parameters of a task") {
                    Task task;

                    TaskCollection actualCommands;
                    Path actualWorkingDir = current_path();
                    EnvironmentCollection actualEnvironment;

                    COMBINATIONS("Append a string") {
                        const string taskPart = "const string task";
                        actualCommands.push_back(taskPart);
                        task.append(taskPart);
                    }

                    COMBINATIONS("Append a moved string") {
                        string taskPart = "const string task";
                        actualCommands.push_back(taskPart);
                        task.append(move(taskPart));
                    }

                    COMBINATIONS("Append a task collection") {
                        const TaskCollection taskPart = {"taskcollection1", "taskcollection2", "taskcollection3"};
                        appendVectors(actualCommands, taskPart);
                        task.append(taskPart);
                    }

                    COMBINATIONS("Move a task collection") {
                        TaskCollection taskPart = {"taskcollection1", "taskcollection2", "taskcollection3"};
                        appendVectors(actualCommands, taskPart);
                        task.append(move(taskPart));
                    }

                    COMBINATIONS("Add a string with the space separator as part of the argument") {
                        const string taskPart = "This is one command in total";
                        actualCommands.push_back(taskPart);
                        task.append(taskPart);
                    }

                    COMBINATIONS("Add a collection with the space separator as part of the argument") {
                        const TaskCollection taskPart = {"This is one command in total", "This is another command that is one"};
                        appendVectors(actualCommands, taskPart);
                        task.append(taskPart);
                    }

                    COMBINATIONS("Set an environment") {
                        const EnvironmentCollection environment = {{"ENV1", "value1"}, {"ENV2", "value2"}};
                        actualEnvironment = environment;
                        task.setEnvironment(environment);
                    }

                    COMBINATIONS("Set a moved environment") {
                        EnvironmentCollection environment = {{"MOVE-ENV1", "value1"}, {"MOVE-ENV2", "value2"}};
                        actualEnvironment = environment;
                        task.setEnvironment(move(environment));
                    }

                    COMBINATIONS("Append an environment value") {
                        EnvironmentValue value1("APPEND1", "append-value1");
                        actualEnvironment.insert(value1);
                        task.appendToEnvironment(move(value1));
                    }

                    COMBINATIONS("Append an environment value twice with a different value") {
                        EnvironmentValue value1("OVERWRITE1", "append-value1");
                        task.appendToEnvironment(move(value1));
                        EnvironmentValue value2("OVERWRITE1", "append-value2");
                        actualEnvironment.insert(value2);
                        task.appendToEnvironment(move(value2));
                    }

                    COMBINATIONS("Append an environment collection") {
                        EnvironmentCollection collection({{"APPENDCOLLECTION1", "append-collection1"}, {"APPENDCOLLECTION2", "append-collection2"}});
                        for(const auto& value : collection) {
                            actualEnvironment.insert(value);
                        }
                        task.appendToEnvironment(move(collection));
                    }

                    COMBINATIONS("Setting the work directory") {
                        actualWorkingDir /= "tmp";
                        task.setWorkingDirectory(actualWorkingDir);
                    }

                    THEN_WHEN("We check the getters") {
                        THEN_CHECK("The commands are correct") {
                            REQUIRE(task.getTask() == actualCommands);
                        }
                        THEN_CHECK("The command string is correct") {
                            REQUIRE(task.toString() == toString(actualCommands));
                        }
                        THEN_CHECK("The environment is correct") {
                            REQUIRE(task.getEnvironment() == actualEnvironment);
                        }
                        THEN_CHECK("The working directory is correct") {
                            REQUIRE(task.getWorkingDirectory() == actualWorkingDir);
                        }
                    }
                }
            }
        }

        SCENARIO("Test the comparison operators", "[task]") {
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
            GIVEN("Two tasks used for equality comparison") {
                MAKE_COMBINATIONS("For different equality situations") {
                    Task task1;
                    Task task2;

                    COMBINATIONS("Append the same commands") {
                        static const TaskCollection actualTask({"task1", "task2"}); 
                        task1.append(actualTask);
                        task2.append(actualTask);
                    }

                    COMBINATIONS("Add the same environment") {
                        static const EnvironmentCollection env({{"ENV1", "value1"}, {"ENV2", "value2"}, {"ENV3", "value3"}});
                        task1.setEnvironment(env);
                        task2.setEnvironment(env);
                    }

                    COMBINATIONS("Add the same working directories") {
                        static const Path workingDirectory("/tmp");
                        task1.setWorkingDirectory(workingDirectory);
                        task2.setWorkingDirectory(workingDirectory);
                    }

                    THEN_WHEN("We compare them") {
                        THEN_CHECK("They should compare equal") {
                            REQUIRE(task1 == task2);
                            REQUIRE_FALSE(task1 != task2);
                        }
                    }
                }
            }

            GIVEN("Two tasks used for equality comparison") {
                MAKE_COMBINATIONS("For different inequality situations") {
                    Task task1;
                    Task task2;

                    COMBINATIONS("Append to one a command") {
                        task1.append("one-command");
                    }

                    COMBINATIONS("Different commands") {
                        task1.append("command1");
                        task2.append("command2");
                    }

                    COMBINATIONS("Different number of commands") {
                        task1.append("command1");
                        task2.append(TaskCollection({"command1", "command2"}));
                    }

                    COMBINATIONS("Commands in different order") {
                        TaskCollection commands({"command1", "command2"});
                        task1.append(commands);
                        task2.append(reverse(commands));
                    }

                    COMBINATIONS("Append to one an additional environment variable") {
                        task1.appendToEnvironment(EnvironmentValue("ONE-ENV", "env1"));
                    }

                    COMBINATIONS("Append different number of environment variables") {
                        const EnvironmentValue value1("ENV1", "value1");
                        EnvironmentValue moveValue1ForTask1(value1);
                        EnvironmentValue moveValue1ForTask2(value1);
                        const EnvironmentValue value2("ENV2", "value1");
                        EnvironmentValue moveValue2ForTask2(value2);
                        task1.appendToEnvironment(move(moveValue1ForTask1));
                        task2.appendToEnvironment(move(moveValue1ForTask2));
                        task2.appendToEnvironment(move(moveValue2ForTask2));
                    }

                    COMBINATIONS("Append environment variables with different keys") {
                        task1.appendToEnvironment(EnvironmentValue("key1", "value"));
                        task1.appendToEnvironment(EnvironmentValue("key2", "value"));
                    }

                    COMBINATIONS("Append environment variables with different values") {
                        task1.appendToEnvironment(EnvironmentValue("key", "value1"));
                        task1.appendToEnvironment(EnvironmentValue("key", "value2"));
                    }

                    COMBINATIONS("Change the working directories") {
                        task1.setWorkingDirectory(current_path());
                        task2.setWorkingDirectory(current_path() / "tmp");
                    }

                    THEN_WHEN("We compare them") {
                        THEN_CHECK("They should not compare equal") {
                            COMBINATIONS_ONLY_IF_BRANCH_TAKEN {
                                REQUIRE(task1 != task2);
                                REQUIRE_FALSE(task1 == task2);
                            }
                        }
                    }
                }
            }
        }
        SCENARIO("Test the streaming operator", "[task]") {
            GIVEN("An empty stream and a task") {
                stringstream stream;
                stringstream actualStream;
                const Path actualWorkingDirectory("/tmp");

                const TaskCollection tasks({"task1", "task2"});
                Task task;
                task.append(tasks);
                const EnvironmentCollection env({{"ENV1", "value1"}, {"ENV2", "value2"}});
                task.setEnvironment(env);
                task.setWorkingDirectory(actualWorkingDirectory);

                WHEN("We stream the task") {
                    stream << task;

                    THEN("The stream should be ok") {
                        REQUIRE(stream.good());
                    }

                    THEN("The stream should have the correct format") {
                        actualStream << "Task {Environment(" << env.size() << "): {";
                        for(const auto& envValue : env) {
                            actualStream << " " << envValue.first << ": " << envValue.second << ";";
                        }
                        actualStream << "} ";

                        actualStream << "Command(" << tasks.size() << "): {";
                        for(const auto& taskPart : tasks) { 
                            actualStream << " " << taskPart;
                        }
                        actualStream << "} ";
                        actualStream << "Working-dir: {" << actualWorkingDirectory.native() << "}";
                        actualStream << "}" << endl;
                        REQUIRE(stream.str() == actualStream.str());
                    }
                }
            }
        }
    } // namespace test
} // namespace core
} // namespace execHelper
