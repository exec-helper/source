#include "unittest/catch.h"

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

            WHEN("We add gen-html settings") {
                ExecutorStub::TaskQueue expectedTasks;
                expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));

                AND_WHEN("We add it as a generic setting") {
                    addSettings(rootSettings[lcovConfigKey], "gen-html", "yes");
                    expectedTasks.emplace_back(Task({"genhtml", "--output-directory", ".", "--title", "Hello", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a specific setting") {
                    addSettings(rootSettings[lcovConfigKey][command], "gen-html", "yes");
                    expectedTasks.emplace_back(Task({"genhtml", "--output-directory", ".", "--title", "Hello", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a generic setting") {
                    addSettings(rootSettings[lcovConfigKey], "gen-html", "no");
                }

                AND_WHEN("We add it as a specific setting") {
                    addSettings(rootSettings[lcovConfigKey][command], "gen-html", "no");
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

    SCENARIO("Test the configuration combinations") {
        MAKE_COMBINATIONS("Of several orthogonal settings") {
            const string command("lcov-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, lcovConfigKey, command);
            addSettings(rootSettings[lcovConfigKey][command], "run-command", "memory");

            Lcov plugin;
            Task task;
    
            ExecutorStub::TaskQueue expectedTasks;
            MemoryHandler memory;

            string infoFile("lcov-plugin.info");
            string baseDirectory(".");
            string directory(".");
            string genHtmlOutputDir(".");
            TaskCollection genHtmlCommandLine;
            string genHtmlTitle("Hello");

            SettingsNode& settings = rootSettings[lcovConfigKey];

            COMBINATION {
                settings = rootSettings[lcovConfigKey][command];
            }

            COMBINATION {
                baseDirectory = "base-dir";
                addSettings(settings, "base-directory", baseDirectory);
            }

            COMBINATION {
                directory = "dir";
                addSettings(settings, "directory", directory);
            }
            
            COMBINATION {
                infoFile = "infoFile.info"; 
                addSettings(settings, "info-file", infoFile);
            }

            COMBINATION {
                addSettings(settings, "zero-counters", "yes");
                expectedTasks.emplace_back(Task({"lcov", "--base-directory", baseDirectory, "--directory", directory, "--zerocounters"}));
            }

            expectedTasks.emplace_back(Task({"lcov", "--base-directory", baseDirectory, "--directory", directory, "--capture", "--output", infoFile}));

            COMBINATION {
                addSettings(settings, "excludes", {"exclude1"});
                expectedTasks.emplace_back(Task({"lcov", "--remove", infoFile, "\"exclude1\"", "--output-file", infoFile}));
            }

            COMBINATION {
                genHtmlOutputDir = "output-dir";
                addSettings(settings, "gen-html-output", genHtmlOutputDir);
            }

            COMBINATION {
                const TaskCollection genHtmlCommandLineValue({"--option1", "--option2"});
                addSettings(settings, "gen-html-command-line", genHtmlCommandLineValue);
                genHtmlCommandLine = genHtmlCommandLineValue;
            }

            COMBINATION {
               genHtmlTitle = "random-title"; 
               addSettings(settings, "gen-html-title", genHtmlTitle);
            }

            COMBINATION {
                addSettings(settings, "gen-html", "yes");
                Task expectedTask({"genhtml", "--output-directory", genHtmlOutputDir, "--title", genHtmlTitle, infoFile});
                expectedTask.append(genHtmlCommandLine);
                expectedTasks.emplace_back(expectedTask);
            }

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
} } }
