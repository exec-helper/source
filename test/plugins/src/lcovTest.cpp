#include "unittest/catch.h"

#include <string>

#include "core/task.h"
#include "plugins/lcov.h"
#include "plugins/memory.h"

#include "utils/utils.h"
#include "optionsStub.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::plugins::Lcov;
using execHelper::plugins::MemoryHandler;

using execHelper::test::OptionsStub;
using execHelper::test::utils::copyAndAppend;
using execHelper::core::test::ExecutorStub;

namespace {
    const string lcovConfigKey("lcov");

    void checkMemories(const MemoryHandler& memory, const Command& command) {
        const MemoryHandler::Memories memories = memory.getExecutions();
        REQUIRE(memories.size() == 1U);
        for(const auto& memoryElement : memories) {
            REQUIRE(memoryElement.command == command);
        }
    }
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
                    REQUIRE_FALSE(returnCode);
                }
            }
        }
    }

    SCENARIO("Test the mandatory settings of the lcov plugin", "[plugins][lcov]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("lcov-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;

            Lcov plugin;
            Task task;

            WHEN("We add the run-command setting") {
                MemoryHandler memory;
                ExecutorStub::TaskQueue expectedTasks;

                AND_WHEN("We add it as a general command") {
                    rootSettings.add({lcovConfigKey, "run-command"}, "memory");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }    

                AND_WHEN("We add it as a specific command") {
                    rootSettings.add({lcovConfigKey, command, "run-command"}, "memory");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }    

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("It should succeed") {
                        REQUIRE(returnCode);
                    }

                    THEN_CHECK("It called the right commands") {
                        REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
                    }

                    THEN_CHECK("It should have called the appropriate plugin") {
                        checkMemories(memory, command);
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

            Lcov plugin;
            Task task;

            WHEN("We add the run-command setting") {
                MemoryHandler memory;
                memory.setReturnCode(false);
                ExecutorStub::TaskQueue expectedTasks;

                AND_WHEN("We add it as a general command") {
                    rootSettings.add({lcovConfigKey, "run-command"}, "memory");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }    

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("It should succeed") {
                        REQUIRE_FALSE(returnCode);
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
            rootSettings.add({lcovConfigKey, "run-command"}, "memory");

            Lcov plugin;
            Task task;
            
            MemoryHandler memory;

            WHEN("We activate or deactivate the zero counters setting") {
                ExecutorStub::TaskQueue expectedTasks;
                AND_WHEN("We add it as a generic setting") {
                    rootSettings.add({lcovConfigKey, "zero-counters"}, "yes");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--zerocounters"}));
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a specific setting") {
                    rootSettings.add({lcovConfigKey, command, "zero-counters"}, "yes");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--zerocounters"}));
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a generic setting") {
                    rootSettings.add({lcovConfigKey, "zero-counters"}, "no");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a specific setting") {
                    rootSettings.add({lcovConfigKey, command, "zero-counters"}, "no");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("It should succeed") {
                        REQUIRE(returnCode);
                    }

                    THEN_CHECK("It called the right commands") {
                        REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
                    }

                    THEN_CHECK("It should have called the appropriate plugin") {
                        checkMemories(memory, command);
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
            rootSettings.add({lcovConfigKey, "run-command"}, "memory");

            Lcov plugin;
            Task task;
            
            MemoryHandler memory;

            WHEN("We add exclude settings") {
                ExecutorStub::TaskQueue expectedTasks;

                AND_WHEN("We add it as a generic setting") {
                    rootSettings.add({lcovConfigKey, "excludes"}, "exclude1");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                    expectedTasks.emplace_back(Task({"lcov", "--remove", "lcov-plugin.info", R"("exclude1")", "--output-file", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a specific setting") {
                    rootSettings.add({lcovConfigKey, command, "excludes"}, "exclude1");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                    expectedTasks.emplace_back(Task({"lcov", "--remove", "lcov-plugin.info", R"("exclude1")", "--output-file", "lcov-plugin.info"}));
                }

                AND_WHEN("We use multiple excludes") {
                    rootSettings.add({lcovConfigKey, "excludes"}, {"exclude1", "exclude2", "exclude3"});
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                    expectedTasks.emplace_back(Task({"lcov", "--remove", "lcov-plugin.info", R"("exclude1")", R"("exclude2")", R"("exclude3")", "--output-file", "lcov-plugin.info"}));
                }

                AND_WHEN("We use no excludes") {
                    rootSettings.add({lcovConfigKey}, "excludes");
                    expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));
                }

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("It should succeed") {
                        REQUIRE(returnCode);
                    }

                    THEN_CHECK("It called the right commands") {
                        REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
                    }

                    THEN_CHECK("It should have called the appropriate plugin") {
                        checkMemories(memory, command);
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
            rootSettings.add({lcovConfigKey, "run-command"}, "memory");

            Lcov plugin;
            Task task;
            
            MemoryHandler memory;

            WHEN("We add gen-html settings") {
                ExecutorStub::TaskQueue expectedTasks;
                expectedTasks.emplace_back(Task({"lcov", "--base-directory", ".", "--directory", ".", "--capture", "--output", "lcov-plugin.info"}));

                AND_WHEN("We add it as a generic setting") {
                    rootSettings.add({lcovConfigKey, "gen-html"}, "yes");
                    expectedTasks.emplace_back(Task({"genhtml", "--output-directory", ".", "--title", "Hello", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a specific setting") {
                    rootSettings.add({lcovConfigKey, command, "gen-html"}, "yes");
                    expectedTasks.emplace_back(Task({"genhtml", "--output-directory", ".", "--title", "Hello", "lcov-plugin.info"}));
                }

                AND_WHEN("We add it as a generic setting") {
                    rootSettings.add({lcovConfigKey, "gen-html"}, "no");
                }

                AND_WHEN("We add it as a specific setting") {
                    rootSettings.add({lcovConfigKey, command, "gen-html"}, "no");
                }

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("It should succeed") {
                        REQUIRE(returnCode);
                    }

                    THEN_CHECK("It called the right commands") {
                        REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
                    }

                    THEN_CHECK("It should have called the appropriate plugin") {
                        checkMemories(memory, command);
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
            rootSettings.add({lcovConfigKey, "run-command"}, "memory");

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

            SettingsNode::SettingsKeys baseSettingsKeys = {lcovConfigKey};

            COMBINATION {
                baseSettingsKeys.push_back(command);
            }

            COMBINATION {
                baseDirectory = "base-dir";
                rootSettings.add(copyAndAppend(baseSettingsKeys, "base-directory"), baseDirectory);
            }

            COMBINATION {
                directory = "dir";
                rootSettings.add(copyAndAppend(baseSettingsKeys, "directory"), directory);
            }
            
            COMBINATION {
                infoFile = "infoFile.info"; 
                rootSettings.add(copyAndAppend(baseSettingsKeys, "info-file"), infoFile);
            }

            COMBINATION {
                rootSettings.add(copyAndAppend(baseSettingsKeys, "zero-counters"), "yes");
                expectedTasks.emplace_back(Task({"lcov", "--base-directory", baseDirectory, "--directory", directory, "--zerocounters"}));
            }

            expectedTasks.emplace_back(Task({"lcov", "--base-directory", baseDirectory, "--directory", directory, "--capture", "--output", infoFile}));

            COMBINATION {
                rootSettings.add(copyAndAppend(baseSettingsKeys, "excludes"), "exclude1");
                expectedTasks.emplace_back(Task({"lcov", "--remove", infoFile, R"("exclude1")", "--output-file", infoFile}));
            }

            COMBINATION {
                genHtmlOutputDir = "output-dir";
                rootSettings.add(copyAndAppend(baseSettingsKeys, "gen-html-output"), genHtmlOutputDir);
            }

            COMBINATION {
                const TaskCollection genHtmlCommandLineValue({"--option1", "--option2"});
                rootSettings.add(copyAndAppend(baseSettingsKeys, "gen-html-command-line"), genHtmlCommandLineValue);
                genHtmlCommandLine = genHtmlCommandLineValue;
            }

            COMBINATION {
               genHtmlTitle = "random-title"; 
               rootSettings.add(copyAndAppend(baseSettingsKeys, "gen-html-title"), genHtmlTitle);
            }

            COMBINATION {
               rootSettings.add(copyAndAppend(baseSettingsKeys, "gen-html"), "yes");
                Task expectedTask({"genhtml", "--output-directory", genHtmlOutputDir, "--title", genHtmlTitle, infoFile});
                expectedTask.append(genHtmlCommandLine);
                expectedTasks.emplace_back(expectedTask);
            }

            bool returnCode = plugin.apply(command, task, options);

            THEN_CHECK("It should succeed") {
                REQUIRE(returnCode);
            }

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
            }

            THEN_CHECK("It should have called the appropriate plugin") {
                checkMemories(memory, command);
            }
        }
    }
} } }
