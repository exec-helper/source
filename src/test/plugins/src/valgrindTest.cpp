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
using execHelper::test::utils::PATTERN1;
using execHelper::test::utils::PATTERN2;
using execHelper::test::utils::PATTERNS;

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
                THEN("As a general command") {
                    addSettings(rootSettings[valgrindConfigKey], "tool", "memcheck");
                }
                THEN("As a specific command") {
                    addSettings(rootSettings[valgrindConfigKey][command], "tool", "memcheck");
                }

                REQUIRE(plugin.apply(command, task, options) == false);
            }

            WHEN("We add the run-command config parameter") {
                const string runCommand("run-command");

                THEN("As a general parameter") {
                    addSettings(rootSettings[valgrindConfigKey], runCommand, "memory");
                }
                THEN("As a specific parameter") {
                    addSettings(rootSettings[valgrindConfigKey][command], runCommand, "memory");
                }

                MemoryHandler memory;

                REQUIRE(plugin.apply(command, task, options) == true);

                Task actualTask;
                actualTask.append("valgrind");
                const Memory::Memories& memories = memory.getExecutions();
                REQUIRE(memories.size() == 1U);
                REQUIRE(memories[0].command == command);
                REQUIRE(memories[0].task == actualTask);
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
                THEN("As a general command") {
                    addSettings(rootSettings[valgrindConfigKey], "tool", toolName);
                }
                THEN("As a specific command") {
                    addSettings(rootSettings[valgrindConfigKey][command], "tool", toolName);
                }

                REQUIRE(plugin.apply(command, task, options) == true);

                Task actualTask;
                actualTask.append("valgrind");
                actualTask.append("--tool=" + toolName);
                const Memory::Memories& memories = memory.getExecutions();
                REQUIRE(memories.size() == 1U);
                REQUIRE(memories[0].command == command);
                REQUIRE(memories[0].task == actualTask);
            }

            WHEN("We call the plugin without a proper tool name") {
                THEN("As a general command") {
                    addSettings(rootSettings, valgrindConfigKey, "tool");
                }
                THEN("As a specific command") {
                    addSettings(rootSettings[valgrindConfigKey], command, "tool");
                }

                REQUIRE(plugin.apply(command, task, options) == true);

                Task actualTask;
                actualTask.append("valgrind");
                const Memory::Memories& memories = memory.getExecutions();
                REQUIRE(memories.size() == 1U);
                REQUIRE(memories[0].task == actualTask);
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

                THEN("As a general command") {
                    addSettings(rootSettings[valgrindConfigKey], "command-line", commandLine);
                }

                THEN("As a specific command") {
                    addSettings(rootSettings[valgrindConfigKey], "command-line", commandLine);
                }

                REQUIRE(plugin.apply(command, task, options));

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

            WHEN("We call the plugin with a proper command-line setting") {
                // Add the pattern to the options
                for(const auto& pattern : PATTERNS) {
                    options.m_patternsHandler->addPattern(pattern);
                }

                const vector<string> commandLine({"{" + PATTERN1.getKey() + "}", "--{" + PATTERN2.getKey() + "}"});

                THEN("As a general command") {
                    // Add the keys to the PATTERNS config value
                    for(const auto& pattern : PATTERNS) {
                        addSettings(rootSettings[valgrindConfigKey], "patterns", pattern.getKey());
                    }
                    addSettings(rootSettings[valgrindConfigKey], "command-line", commandLine);
                }
                THEN("As a specific command") {
                    // Add the keys to the patterns config value
                    for(const auto& pattern : PATTERNS) {
                        addSettings(rootSettings[valgrindConfigKey][command], "patterns", pattern.getKey());
                    }
                    addSettings(rootSettings[valgrindConfigKey][command], "command-line", commandLine);
                }

                REQUIRE(plugin.apply(command, task, options) == true);

                vector<Task> actualTasks;
                for(const auto& PATTERN1Values : PATTERN1.getDefaultValues()) {
                    for(const auto& PATTERN2Values : PATTERN2.getDefaultValues()) {
                        Task actualTask;
                        actualTask.append("valgrind");
                        actualTask.append(PATTERN1Values);
                        actualTask.append("--" + PATTERN2Values);
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
