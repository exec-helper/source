#include <fstream>
#include <string>
#include <vector>

#include <catch.hpp>

#include "core/task.h"
#include "core/execHelperOptions.h"
#include "executorStub.h"

#include "plugins/commandLineCommand.h"

using std::ofstream;
using std::string;
using std::endl;
using std::vector;
using execHelper::core::Task;
using execHelper::core::ExecHelperOptions;
using execHelper::core::test::ExecutorStub;

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the command line command plugin", "[plugins][commandlinecommand]") {
        GIVEN("A configuration file and tasks we want to execute") {
            string commandLineToExecute1("echo Hello");
            string commandLineToExecute2("echo world!!!");

            Task actualTasks;
            actualTasks.append(commandLineToExecute1);
            actualTasks.append(commandLineToExecute2);

            string filename("test.exec-helper");
            ofstream configFile;
            configFile.open(filename);
            configFile << "command-line-command:" << endl 
                       <<"    command:" << endl
                       << "        - " << commandLineToExecute1 << endl
                       << "        - " << commandLineToExecute2 << endl;

            configFile.close();
            
            CommandLineCommand plugin;

            WHEN("We apply the task to the command line command plugin") {
                Task task;
                ExecHelperOptions options;
                options.parseSettingsFile(filename);

                ExecutorStub executor;
                options.setExecutor(&executor);

                plugin.apply("random-command", task, options); 

                THEN("We should get the specifics of the Debug mode object") {
                    REQUIRE(executor.getExecutedTasks() == vector<Task>({actualTasks})); 
                }
            }
        }
    }
} } }
