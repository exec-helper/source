#include <catch.hpp>

#include <vector>

#include "core/task.h"
#include "core/pattern.h"
#include "plugins/valgrind.h"
#include "plugins/memory.h"

#include "utils/utils.h"
#include "optionsStub.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::Pattern;
using execHelper::plugins::Valgrind;
using execHelper::plugins::Memory;
using execHelper::plugins::MemoryHandler;

using execHelper::test::OptionsStub;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::Patterns;

namespace {
    const string valgrindConfigKey("valgrind");
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Test the default options of the valgrind plugin", "[plugins][valgrind]") {
        GIVEN("A default configuration and the plugin") {
            Valgrind plugin;

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

    SCENARIO("Test the mandatory options of the valgrind plugin", "[plugins][valgrind]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("valgrind-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, valgrindConfigKey, command);

            Valgrind plugin;
            Task task;

            WHEN("We add the tool config parameter") {
                addSettings(rootSettings[valgrindConfigKey], "tool", "memcheck");

                bool returnCode = plugin.apply(command, task, options);

                THEN("It should fail") {
                    REQUIRE(returnCode == false);
                }
            }

            WHEN("We add the run-command config parameter") {
                const string runCommand("run-command");
                addSettings(rootSettings[valgrindConfigKey], command, runCommand);
                addSettings(rootSettings[valgrindConfigKey][command], runCommand, "memory");

                MemoryHandler memory;

                bool returnCode = plugin.apply(command, task, options);

                THEN("It should succeed") {
                    REQUIRE(returnCode == true);
                }
                THEN("The run-command should be called") {
                    Task actualTask;
                    actualTask.append("valgrind");

                    const Memory::Memories& memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].command == command);
                    REQUIRE(memories[0].task == actualTask);
                }
            }
        }
    }

    SCENARIO("Test configuration issues behaviour", "[plugins][valgrind]") {
        GIVEN("A default configuration") {
            const string command("valgrind-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, valgrindConfigKey, command);

            Valgrind plugin;
            Task task;

            WHEN("We add the run-command config parameter without a value") {
                const string runCommand("run-command");
                addSettings(rootSettings[valgrindConfigKey], command, runCommand);

                bool returnCode = plugin.apply(command, task, options);

                THEN("It should fail") {
                    REQUIRE(returnCode == false);
                }
            }
        }
    }

    SCENARIO("Test the tool config parameter", "[plugins][valgrind]") {
        GIVEN("A valid configuration with the tool config parameter") {
            const string command("valgrind-command");
            const string runCommand("run-command");
            const string toolName("tool-name");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, valgrindConfigKey, command);
            addSettings(rootSettings[valgrindConfigKey], command, runCommand);
            addSettings(rootSettings[valgrindConfigKey][command], runCommand, "memory");

            Valgrind plugin;
            Task task;

            MemoryHandler memory;

            WHEN("We call the plugin with a proper tool name") {
                addSettings(rootSettings[valgrindConfigKey], "tool", toolName);

                bool returnCode = plugin.apply(command, task, options);

                THEN("It should succeed") {
                    REQUIRE(returnCode == true); 
                }
                THEN("The task should contain the associated parameter") {
                    Task actualTask;
                    actualTask.append("valgrind");
                    actualTask.append("--tool=" + toolName);

                    const Memory::Memories& memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].command == command);
                    REQUIRE(memories[0].task == actualTask);
                }
            }

            WHEN("We call the plugin without a proper tool name") {
                bool returnCode = plugin.apply(command, task, options);

                THEN("It should succeed") {
                    REQUIRE(returnCode == true);
                }
                THEN("The task should not contain the associated parameter") {
                    Task actualTask;
                    actualTask.append("valgrind");

                    const Memory::Memories& memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].task == actualTask);
                }
            }
        }
    }

    SCENARIO("Test the command-line config parameter of the valgrind plugin", "[plugins][valgrind]") {
        GIVEN("A valid configuration with the command-line config parameter") {
            const string command("valgrind-command");
            const string runCommand("run-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, valgrindConfigKey, command);
            addSettings(rootSettings[valgrindConfigKey], command, runCommand);
            addSettings(rootSettings[valgrindConfigKey][command], runCommand, "memory");

            Valgrind plugin;
            Task task;

            MemoryHandler memory;

            WHEN("We call the plugin with a proper command-line setting") {
                const vector<string> commandLine({"arg1", "--arg2"});
                addSettings(rootSettings[valgrindConfigKey], "command-line", commandLine);

                bool returnCode = plugin.apply(command, task, options);

                THEN("It should succeed") {
                    REQUIRE(returnCode == true); 
                }
                THEN("The task should contain the associated parameter") {
                    Task actualTask;
                    actualTask.append("valgrind");
                    for(const auto& line : commandLine) {
                        actualTask.append(line);
                    }

                    const Memory::Memories& memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].command == command);
                    REQUIRE(memories[0].task == actualTask);
                }
            }

            WHEN("We call the plugin with a proper command-line setting") {
                Pattern pattern1("PATTERN1", {"pattern1"}, 'p', "--pattern1");
                Pattern pattern2("PATTERN2", {"pattern2a", "pattern2b"}, 'q', "--pattern2");
                Patterns patterns({pattern1, pattern2});
                // Add the keys to the patterns config value
                for(const auto& pattern : patterns) {
                    addSettings(rootSettings[valgrindConfigKey], "patterns", pattern.getKey());
                }
                // Add the pattern to the options
                for(const auto& pattern : patterns) {
                    options.m_patternsHandler->addPattern(pattern);
                }

                const vector<string> commandLine({"{" + pattern1.getKey() + "}", "--{" + pattern2.getKey() + "}"});
                addSettings(rootSettings[valgrindConfigKey], "command-line", commandLine);

                bool returnCode = plugin.apply(command, task, options);

                THEN("It should succeed") {
                    REQUIRE(returnCode == true); 
                }
                THEN("The task should contain the associated parameter") {
                    vector<Task> actualTasks;
                    for(const auto& pattern1Values : pattern1.getDefaultValues()) {
                        for(const auto& pattern2Values : pattern2.getDefaultValues()) {
                            Task actualTask;
                            actualTask.append("valgrind");
                            actualTask.append(pattern1Values);
                            actualTask.append("--" + pattern2Values);
                            actualTasks.emplace_back(actualTask);
                        }
                    }

                    const Memory::Memories& memories = memory.getExecutions();
                    REQUIRE(memories.size() == actualTasks.size());
                    for(size_t i = 0; i < actualTasks.size(); ++i) {
                        REQUIRE(memories[i].command == command);
                        REQUIRE(memories[i].task == actualTasks[i]);
                    }
                }
            }
        }
    }

    SCENARIO("Test a failing run command", "[plugins][valgrind]") {
        GIVEN("A default configuration and a run command that will fail") {
            const string command("valgrind-command");
            const string runCommand("run-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, valgrindConfigKey, command);
            addSettings(rootSettings[valgrindConfigKey], command, runCommand);
            addSettings(rootSettings[valgrindConfigKey][command], runCommand, "memory");
            addSettings(rootSettings[valgrindConfigKey], "tool", "memcheck");

            Valgrind plugin;
            Task task;

            MemoryHandler memory;
            memory.setReturnCode(false);

            WHEN("We add the run-command and tool config parameter") {
                bool returnCode = plugin.apply(command, task, options);

                THEN("It should succeed") {
                    REQUIRE(returnCode == false);
                }
                THEN("The run-command should be called") {
                    const Memory::Memories& memories = memory.getExecutions();
                    REQUIRE(memories.size() == 1U);
                    REQUIRE(memories[0].command == command);
                }
            }
        }
    }
} } }
