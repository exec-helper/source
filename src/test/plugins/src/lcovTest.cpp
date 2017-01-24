#include <catch.hpp>
#define THEN_WHEN(x)
#define THEN_CHECK(x)

#include <string>
#include <vector>

#include "core/task.h"
#include "core/pattern.h"
#include "plugins/lcov.h"
#include "plugins/memory.h"

#include "utils/utils.h"
#include "optionsStub.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::Pattern;
using execHelper::plugins::Lcov;
using execHelper::plugins::MemoryHandler;

using execHelper::test::OptionsStub;
using execHelper::test::utils::addSettings;
using execHelper::core::test::ExecutorStub;

namespace {
    const string lcovConfigKey("lcov");
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Test the default options of the lcov plugin", "[plugins][lcov]") {
        GIVEN("A default configuration and the plugin") {
            Lcov plugin;

            Task task;
            const OptionsStub options;

            WHEN("We call the plugin") {
                bool returnCode = plugin.apply("random-command", task, options);

                THEN("The call should fail") {
                    REQUIRE(returnCode == false);
                }
            }
        }
    }

    SCENARIO("Test the mandatory settings of the lcov plugin", "[plugins][lcov]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("lcov-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, lcovConfigKey, command);

            Lcov plugin;
            Task task;

            WHEN("We add the run-command setting") {
                MemoryHandler memory;
                ExecutorStub::TaskQueue expectedTasks;

                AND_WHEN("We add it as a general command") {
                    addSettings(rootSettings[lcovConfigKey], "run-command", "memory");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }    

                AND_WHEN("We add it as a specific command") {
                    addSettings(rootSettings[lcovConfigKey][command], "run-command", "memory");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }    

                AND_WHEN("We add a command line") {
                    const TaskCollection commandLine({"--option1", "--option2"});
                    addSettings(rootSettings[lcovConfigKey][command], "run-command", "memory");
                    addSettings(rootSettings[lcovConfigKey], "command-line", commandLine);

                    Task expectedTask({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"});
                    expectedTask.append(commandLine);
                    expectedTasks.emplace_back(expectedTask);
                }

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("It should succeed") {
                        REQUIRE(returnCode == true);
                    }

                    THEN_CHECK("It called the right commands") {
                        const ExecutorStub::TaskQueue executedTasks = options.m_executor.getExecutedTasks(); 

                        REQUIRE(expectedTasks == executedTasks);
                    }

                    THEN_CHECK("It should have called the appropriate plugin") {
                        const Memory::Memories memories = memory.getExecutions();
                        REQUIRE(memories.size() == 1U);
                        for(const auto& memoryElement : memories) {
                            REQUIRE(memoryElement.command == command);
                        }
                    }
                }
            }
        }
    }

    SCENARIO("Test a failing run-command", "[plugins][lcov]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("lcov-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, lcovConfigKey, command);

            Lcov plugin;
            Task task;

            WHEN("We add the run-command setting") {
                MemoryHandler memory;
                memory.setReturnCode(false);
                ExecutorStub::TaskQueue expectedTasks;

                AND_WHEN("We add it as a general command") {
                    addSettings(rootSettings[lcovConfigKey], "run-command", "memory");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }    

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("It should succeed") {
                        REQUIRE(returnCode == false);
                    }
                }
            }
        }
    }

    SCENARIO("Test the zero counters setting", "[plugins][lcov]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("lcov-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, lcovConfigKey, command);
            addSettings(rootSettings[lcovConfigKey], "run-command", "memory");

            Lcov plugin;
            Task task;
            
            MemoryHandler memory;

            WHEN("We activate or deactivate the zero counters setting") {
                ExecutorStub::TaskQueue expectedTasks;
                AND_WHEN("We add it as a generic setting") {
                    addSettings(rootSettings[lcovConfigKey], "zero-counters", "yes");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--zerocounters"}));
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a specific setting") {
                    addSettings(rootSettings[lcovConfigKey][command], "zero-counters", "yes");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--zerocounters"}));
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a generic setting") {
                    addSettings(rootSettings[lcovConfigKey], "zero-counters", "no");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a specific setting") {
                    addSettings(rootSettings[lcovConfigKey][command], "zero-counters", "no");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }

                AND_WHEN("We add a command line") {
                    const TaskCollection commandLine({"--option1", "--option2"});
                    addSettings(rootSettings[lcovConfigKey], "zero-counters", "yes");
                    addSettings(rootSettings[lcovConfigKey], "command-line", commandLine);

                    Task expectedTask({"lcov", "--base-directory", ".", "--directory", ".", "--zerocounters"});
                    expectedTask.append(commandLine);
                    expectedTasks.emplace_back(expectedTask);
                    
                    Task captureTask({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"});
                    captureTask.append(commandLine);
                    expectedTasks.emplace_back(captureTask);
                }


                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("It should succeed") {
                        REQUIRE(returnCode == true);
                    }

                    THEN_CHECK("It called the right commands") {
                        const ExecutorStub::TaskQueue executedTasks = options.m_executor.getExecutedTasks(); 

                        REQUIRE(expectedTasks == executedTasks);
                    }

                    THEN_CHECK("It should have called the appropriate plugin") {
                        const Memory::Memories memories = memory.getExecutions();
                        REQUIRE(memories.size() == 1U);
                        for(const auto& memoryElement : memories) {
                            REQUIRE(memoryElement.command == command);
                        }
                    }
                }
            }
        }
    }

    SCENARIO("Test the exclude setting", "[plugins][lcov]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("lcov-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, lcovConfigKey, command);
            addSettings(rootSettings[lcovConfigKey], "run-command", "memory");

            Lcov plugin;
            Task task;
            
            MemoryHandler memory;

            WHEN("We add exclude settings") {
                ExecutorStub::TaskQueue expectedTasks;

                AND_WHEN("We add it as a generic setting") {
                    addSettings(rootSettings[lcovConfigKey], "excludes", {"exclude1"});
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                    expectedTasks.emplace_back(Task({"lcov", "--remove", "lcov-plugin.info", "\"exclude1\"", "--output-file", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a specific setting") {
                    addSettings(rootSettings[lcovConfigKey][command], "excludes", {"exclude1"});
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                    expectedTasks.emplace_back(Task({"lcov", "--remove", "lcov-plugin.info", "\"exclude1\"", "--output-file", "lcov-plugin.info"}));
                }

                AND_WHEN("We use multiple excludes") {
                    addSettings(rootSettings[lcovConfigKey], "excludes", {"exclude1", "exclude2", "exclude3"});
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                    expectedTasks.emplace_back(Task({"lcov", "--remove", "lcov-plugin.info", "\"exclude1\"", "\"exclude2\"", "\"exclude3\"", "--output-file", "lcov-plugin.info"}));
                }

                AND_WHEN("We use no excludes") {
                    addSettings(rootSettings[lcovConfigKey], "excludes", {});
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }

                AND_WHEN("We add a command line") {
                    const TaskCollection commandLine({"--option1", "--option2"});
                    addSettings(rootSettings[lcovConfigKey], "excludes", {"exclude1", "exclude2", "exclude3"});
                    addSettings(rootSettings[lcovConfigKey], "command-line", commandLine);
                    Task captureTask({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"});
                    captureTask.append(commandLine);
                    expectedTasks.emplace_back(captureTask);

                    Task expectedTask({"lcov", "--remove", "lcov-plugin.info", "\"exclude1\"", "\"exclude2\"", "\"exclude3\"", "--output-file", "lcov-plugin.info"});
                    expectedTask.append(commandLine);
                    expectedTasks.emplace_back(expectedTask);
                }

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("It should succeed") {
                        REQUIRE(returnCode == true);
                    }

                    THEN_CHECK("It called the right commands") {
                        const ExecutorStub::TaskQueue executedTasks = options.m_executor.getExecutedTasks(); 

                        REQUIRE(expectedTasks == executedTasks);
                    }

                    THEN_CHECK("It should have called the appropriate plugin") {
                        const Memory::Memories memories = memory.getExecutions();
                        REQUIRE(memories.size() == 1U);
                        for(const auto& memoryElement : memories) {
                            REQUIRE(memoryElement.command == command);
                        }
                    }
                }
            }
        }
    }

    SCENARIO("Test the gen-html setting", "[plugins][lcov]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("lcov-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, lcovConfigKey, command);
            addSettings(rootSettings[lcovConfigKey], "run-command", "memory");

            Lcov plugin;
            Task task;
            
            MemoryHandler memory;

            WHEN("We add exclude settings") {
                ExecutorStub::TaskQueue expectedTasks;
                expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));

                AND_WHEN("We add it as a generic setting") {
                    addSettings(rootSettings[lcovConfigKey], "gen-html", "yes");
                    expectedTasks.emplace_back(Task({"genhtml", "--output-directory", ".", "--title", "hello", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a specific setting") {
                    addSettings(rootSettings[lcovConfigKey][command], "gen-html", "yes");
                    expectedTasks.emplace_back(Task({"genhtml", "--output-directory", ".", "--title", "hello", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a generic setting") {
                    addSettings(rootSettings[lcovConfigKey], "gen-html", "no");
                }

                AND_WHEN("We add it as a specific setting") {
                    addSettings(rootSettings[lcovConfigKey][command], "gen-html", "no");
                }

                AND_WHEN("We add all the gen-html related settings") {
                    const string genHtmlOutputDir("output-dir");
                    const TaskCollection getHtmlCommandLine({"--option1", "--option2"});

                    addSettings(rootSettings[lcovConfigKey], "gen-html", "yes");
                    addSettings(rootSettings[lcovConfigKey], "gen-html-output", genHtmlOutputDir);
                    addSettings(rootSettings[lcovConfigKey], "gen-html-command-line", getHtmlCommandLine);

                    Task genHtmlTask({"genhtml", "--output-directory", genHtmlOutputDir, "--title", "hello", "lcov-plugin.info"});
                    genHtmlTask.append(getHtmlCommandLine);
                    expectedTasks.emplace_back(genHtmlTask);
                }

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("It should succeed") {
                        REQUIRE(returnCode == true);
                    }

                    THEN_CHECK("It called the right commands") {
                        const ExecutorStub::TaskQueue executedTasks = options.m_executor.getExecutedTasks(); 

                        REQUIRE(expectedTasks == executedTasks);
                    }

                    THEN_CHECK("It should have called the appropriate plugin") {
                        const Memory::Memories memories = memory.getExecutions();
                        REQUIRE(memories.size() == 1U);
                        for(const auto& memoryElement : memories) {
                            REQUIRE(memoryElement.command == command);
                        }
                    }
                }
            }
        }
    }
} } }
