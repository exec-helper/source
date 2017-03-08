#include <catch.hpp>

#include <vector>

#include "core/task.h"
#include "core/pattern.h"
#include "plugins/pmd.h"

#include "utils/utils.h"
#include "optionsStub.h"
#include "executorStub.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::Pattern;
using execHelper::plugins::Pmd;

using execHelper::test::OptionsStub;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::PATTERN1;
using execHelper::test::utils::PATTERN2;
using execHelper::test::utils::PATTERNS;
using execHelper::test::utils::toString;
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
                    REQUIRE(returnCode == false);
                }
            }
        }
    }

    SCENARIO("Test the mandatory options of the pmd plugin", "[plugins][valgrind]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("pmd-command");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, pmdConfigKey, command);

            Pmd plugin;
            Task task;

            WHEN("We add the tool config parameter") {
                AND_WHEN("We use a general parameter") {
                    addSettings(rootSettings[pmdConfigKey], "tool", "tool");
                }
                AND_WHEN("We use a specific parameter") {
                    addSettings(rootSettings[pmdConfigKey][command], "tool", "tool");
                }

                THEN_WHEN("We apply the plugin") {
                    bool returnCode = plugin.apply(command, task, options);

                    THEN_CHECK("That the apply succeeds") {
                        REQUIRE(returnCode == false);
                    }
                }
            }

            WHEN("We add the exec config parameter") {
                THEN("As a general parameter") {
                    addSettings(rootSettings[pmdConfigKey], "exec", "pmd.sh");
                }
                THEN("As a specific parameter") {
                    addSettings(rootSettings[pmdConfigKey][command], "exec", "pmd.sh");
                }
                REQUIRE(plugin.apply(command, task, options) == false);
            }

            WHEN("We add all mandatory config parameters and apply command ") {
                THEN("As a general command") {
                    addSettings(rootSettings[pmdConfigKey], "tool", "tool");
                    addSettings(rootSettings[pmdConfigKey], "exec", "pmd.sh");
                }
                THEN("As a specific command") {
                    addSettings(rootSettings[pmdConfigKey][command], "tool", "tool");
                    addSettings(rootSettings[pmdConfigKey][command], "exec", "pmd.sh");
                }

                REQUIRE(plugin.apply(command, task, options) == true);

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({"pmd.sh", "tool"});
                actualTasks.emplace_back(actualTask);
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }
        }
    }

    SCENARIO("Test the language config options", "[plugins][valgrind]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("pmd-command");
            const string languageConfigKey("language");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, pmdConfigKey, command);
            addSettings(rootSettings[pmdConfigKey], "tool", "cpd");
            addSettings(rootSettings[pmdConfigKey], "exec", "pmd.sh");

            Pmd plugin;
            Task task;

            WHEN("We define the language and run the plugin") {
                const string languageValue("language1");

                THEN("As a general command") {
                    addSettings(rootSettings[pmdConfigKey], languageConfigKey, languageValue);
                }
                THEN("As a specific command") {
                    addSettings(rootSettings[pmdConfigKey][command], languageConfigKey, languageValue);
                }

                REQUIRE(plugin.apply(command, task, options) == true);

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({"pmd.sh", "cpd", "--language", languageValue});
                actualTasks.emplace_back(actualTask);
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }

            WHEN("We do not define the language for the general commands and run the plugin") {
                THEN("As a general command") {
                    addSettings(rootSettings, pmdConfigKey, languageConfigKey);
                }
                THEN("As a specific command") {
                    addSettings(rootSettings[pmdConfigKey], command, languageConfigKey);
                }

                REQUIRE(plugin.apply(command, task, options) == true);

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({"pmd.sh", "cpd"});
                actualTasks.emplace_back(actualTask);
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }
        }
    }

    SCENARIO("Test the cpd tool config options", "[plugins][valgrind]") {
        GIVEN("The respective configuration and the plugin") {
            const string command("pmd-command");
            const string minimumTokensConfigKey("minimum-tokens");
            const string filesConfigKey("files");

            OptionsStub options;
            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, pmdConfigKey, command);
            addSettings(rootSettings[pmdConfigKey], "tool", "cpd");
            addSettings(rootSettings[pmdConfigKey], "exec", "pmd.sh");

            const string minimumTokensValue("minimum-tokens-value");
            const vector<string> filesValue({"file-value1", "file-value2"});

            Pmd plugin;
            Task task;

            WHEN("We define the minimum tokens and files config settings and run the plugin") {
                THEN("As a general command") {
                    addSettings(rootSettings[pmdConfigKey][command], minimumTokensConfigKey, minimumTokensValue);
                    addSettings(rootSettings[pmdConfigKey][command], filesConfigKey, filesValue);
                }
                THEN("As a specific command") {
                    addSettings(rootSettings[pmdConfigKey], minimumTokensConfigKey, minimumTokensValue);
                    addSettings(rootSettings[pmdConfigKey], filesConfigKey, filesValue);
                }

                REQUIRE(plugin.apply(command, task, options) == true);

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({"pmd.sh", "cpd", "--minimum-tokens", minimumTokensValue});
                for(const auto& fileValue : filesValue) {
                    actualTask.append("--files");
                    actualTask.append(fileValue);
                }
                actualTasks.emplace_back(actualTask);
                REQUIRE(actualTasks == options.m_executor.getExecutedTasks());
            }

            WHEN("We do not define the minimum tokens config settings and run the plugin") {
                THEN("As a general command") {
                    addSettings(rootSettings, pmdConfigKey, minimumTokensConfigKey);
                    addSettings(rootSettings, pmdConfigKey, filesConfigKey);
                }
                THEN("As a specific command") {
                    addSettings(rootSettings[pmdConfigKey], command, minimumTokensConfigKey);
                    addSettings(rootSettings[pmdConfigKey], command, filesConfigKey);
                }

                REQUIRE(plugin.apply(command, task, options) == true);

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({"pmd.sh", "cpd"});
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
            addSettings(rootSettings, pmdConfigKey, command);
            addSettings(rootSettings[pmdConfigKey], "tool", "tool1");
            addSettings(rootSettings[pmdConfigKey], "exec", "pmd.sh");

            Pmd plugin;
            Task task;

            WHEN("We call the plugin with a proper command-line setting commands") {
                const vector<string> commandLine({"arg1", "--arg2"});

                THEN("As a general command") {
                    addSettings(rootSettings[pmdConfigKey], "command-line", commandLine);
                }
                THEN("As a specific command") {
                    addSettings(rootSettings[pmdConfigKey][command], "command-line", commandLine);
                }

                REQUIRE(plugin.apply(command, task, options) == true);

                ExecutorStub::TaskQueue actualTasks;
                Task actualTask({"pmd.sh", "tool1", "arg1", "--arg2"});
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
                        addSettings(rootSettings[pmdConfigKey], "patterns", pattern.getKey());
                    }
                    addSettings(rootSettings[pmdConfigKey], "command-line", commandLine);
                }
                THEN("As a specific command") {
                    // Add the keys to the patterns config value
                    for(const auto& pattern : PATTERNS) {
                        addSettings(rootSettings[pmdConfigKey][command], "patterns", pattern.getKey());
                    }
                    addSettings(rootSettings[pmdConfigKey][command], "command-line", commandLine);
                }

                REQUIRE(plugin.apply(command, task, options) == true);

                vector<Task> actualTasks;
                for(const auto& PATTERN1Values : PATTERN1.getDefaultValues()) {
                    for(const auto& PATTERN2Values : PATTERN2.getDefaultValues()) {
                        Task actualTask({"pmd.sh", "tool1"});
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
