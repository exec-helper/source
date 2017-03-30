#include <vector>

#include <catch.hpp>

#include "core/pattern.h"
#include "core/task.h"
#include "plugins/pmd.h"

#include "utils/utils.h"
#include "executorStub.h"
#include "optionsStub.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::plugins::Pmd;

using execHelper::test::OptionsStub;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::PATTERN1;
using execHelper::test::utils::PATTERN2;
using execHelper::test::utils::PATTERNS;
using execHelper::core::test::ExecutorStub;

namespace {
    const string pmdConfigKey("pmd");
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Test the default options of the pmd plugin", "[plugins][pmd]") {
        GIVEN("A default configuration and the plugin") {
            Pmd plugin;

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

    SCENARIO("Test the mandatory options of the pmd plugin", "[plugins][pmd]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("pmd-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;

            Pmd plugin;
            Task task;

            WHEN("We add the tool config parameter") {
                AND_WHEN("We use a general parameter") {
                    rootSettings.add({pmdConfigKey, "tool"}, "tool");
                }
                AND_WHEN("We use a specific parameter") {
                    rootSettings.add({pmdConfigKey, command, "tool"}, "tool");
                }

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("That the apply succeeds") {
                        REQUIRE_FALSE(returnCode);
                    }
                }
            }

            WHEN("We add the exec config parameter") {
                THEN("As a general parameter") {
                    rootSettings.add({pmdConfigKey, "exec"}, "pmd.sh");
                }
                THEN("As a specific parameter") {
                    rootSettings.add({pmdConfigKey, command, "exec"}, "pmd.sh");
                }
                REQUIRE_FALSE(plugin.apply(command, task, options));
            }

            WHEN("We add all mandatory config parameters and apply command ") {
                THEN("As a general command") {
                    rootSettings.add({pmdConfigKey, "tool"}, "tool");
                    rootSettings.add({pmdConfigKey, "exec"}, "pmd.sh");
                }
                THEN("As a specific command") {
                    rootSettings.add({pmdConfigKey, command, "tool"}, "tool");
                    rootSettings.add({pmdConfigKey, command, "exec"}, "pmd.sh");
                }

                REQUIRE(plugin.apply(command, task, options));

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({"pmd.sh", "tool"});
                actualTasks.emplace_back(actualTask);
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }
        }
    }

    SCENARIO("Test the language config options", "[plugins][pmd]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("pmd-command");
            const string languageConfigKey("language");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            const string binary("pmd.sh");
            const string tool("tool1");
            rootSettings.add({pmdConfigKey, "tool"}, tool);
            rootSettings.add({pmdConfigKey, "exec"}, binary);

            Pmd plugin;
            Task task;

            WHEN("We define the language and run the plugin") {
                const string languageValue("language1");

                THEN("As a general command") {
                    rootSettings.add({pmdConfigKey, languageConfigKey}, languageValue);
                }
                THEN("As a specific command") {
                    rootSettings.add({pmdConfigKey, command, languageConfigKey}, languageValue);
                }

                REQUIRE(plugin.apply(command, task, options));

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({"pmd.sh", tool, "--language", languageValue});
                actualTasks.emplace_back(actualTask);
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }

            WHEN("We do not define the language for the general commands and run the plugin") {
                THEN("As a general command") {
                    rootSettings.add({pmdConfigKey}, languageConfigKey);
                }
                THEN("As a specific command") {
                    rootSettings.add({pmdConfigKey, command}, languageConfigKey);
                }

                REQUIRE(plugin.apply(command, task, options));

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({binary, tool});
                actualTasks.emplace_back(actualTask);
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }
        }
    }

    SCENARIO("Test the cpd tool config options", "[plugins][pmd]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("pmd-command");
            const string minimumTokensConfigKey("minimum-tokens");
            const string filesConfigKey("files");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            const string tool("cpd");
            const string binary("pmd.sh");
            rootSettings.add({pmdConfigKey, "tool"}, tool);
            rootSettings.add({pmdConfigKey, "exec"}, binary);

            const string minimumTokensValue("minimum-tokens-value");
            const vector<string> filesValue({"file-value1", "file-value2"});

            Pmd plugin;
            Task task;

            WHEN("We define the minimum tokens and files config settings and run the plugin") {
                THEN("As a general command") {
                    rootSettings.add({pmdConfigKey, minimumTokensConfigKey}, minimumTokensValue);
                    rootSettings.add({pmdConfigKey, filesConfigKey}, filesValue);
                }
                THEN("As a specific command") {
                    rootSettings.add({pmdConfigKey, command, minimumTokensConfigKey}, minimumTokensValue);
                    rootSettings.add({pmdConfigKey, command, filesConfigKey}, filesValue);
                }

                REQUIRE(plugin.apply(command, task, options));

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({binary, tool, "--minimum-tokens", minimumTokensValue});
                for(const auto& fileValue : filesValue) {
                    actualTask.append("--files");
                    actualTask.append(fileValue);
                }
                actualTasks.emplace_back(actualTask);
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }

            WHEN("We do not define the minimum tokens config settings and run the plugin") {
                THEN("As a general command") {
                    rootSettings.add({pmdConfigKey}, minimumTokensConfigKey);
                    rootSettings.add({pmdConfigKey}, filesConfigKey);
                }
                THEN("As a specific command") {
                    rootSettings.add({pmdConfigKey, command}, minimumTokensConfigKey);
                    rootSettings.add({pmdConfigKey, command}, filesConfigKey);
                }

                REQUIRE(plugin.apply(command, task, options));

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({binary, tool});
                actualTasks.emplace_back(actualTask);
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }
        }
    }

    SCENARIO("Test the command-line config parameter of the pmd plugin", "[plugins][pmd]") {
        GIVEN("A valid configuration with the command-line config parameter") {
            const string command("pmd-command");
            const string runCommand("run-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            const string tool("tool1");
            const string binary("pmd.sh");
            rootSettings.add({pmdConfigKey, "tool"}, tool);
            rootSettings.add({pmdConfigKey, "exec"}, binary);

            Pmd plugin;
            Task task;

            WHEN("We call the plugin with a proper command-line setting commands") {
                const vector<string> commandLine({"arg1", "--arg2"});

                THEN("As a general command") {
                    rootSettings.add({pmdConfigKey, "command-line"}, commandLine);
                }
                THEN("As a specific command") {
                    rootSettings.add({pmdConfigKey, command, "command-line"}, commandLine);
                }

                REQUIRE(plugin.apply(command, task, options));

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({binary, tool});
                actualTask.append(commandLine); 
                actualTasks.emplace_back(actualTask);
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }

            WHEN("We call the plugin with a proper command-line setting for the general commands") {
                // Add the pattern to the options
                for(const auto& pattern : PATTERNS) {
                    options.m_patternsHandler->addPattern(pattern);
                }

                const vector<string> commandLine({"{" + PATTERN1.getKey() + "}", "--{" + PATTERN2.getKey() + "}"});

                THEN("As a general command") {
                    // Add the keys to the patterns config value
                    for(const auto& pattern : PATTERNS) {
                        rootSettings.add({pmdConfigKey, "patterns"}, pattern.getKey());
                    }
                    rootSettings.add({pmdConfigKey, "command-line"}, commandLine);
                }
                THEN("As a specific command") {
                    // Add the keys to the patterns config value
                    for(const auto& pattern : PATTERNS) {
                        rootSettings.add({pmdConfigKey, command, "patterns"}, pattern.getKey());
                    }
                    rootSettings.add({pmdConfigKey, command, "command-line"}, commandLine);
                }

                REQUIRE(plugin.apply(command, task, options));

                vector<Task> actualTasks;
                for(const auto& PATTERN1Values : PATTERN1.getDefaultValues()) {
                    for(const auto& PATTERN2Values : PATTERN2.getDefaultValues()) {
                        Task actualTask({binary, tool});
                        actualTask.append(PATTERN1Values);
                        actualTask.append("--" + PATTERN2Values);
                        actualTasks.emplace_back(actualTask);
                    }
                }
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }
        }
    }
} } }
