#include <vector>

#include <gsl/string_span>

#include "core/pattern.h"
#include "core/task.h"
#include "plugins/pluginUtils.h"
#include "plugins/pmd.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::vector;
using std::string;

using gsl::czstring;

using execHelper::config::SettingsNode;
using execHelper::core::Pattern;
using execHelper::core::PatternKeys;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::plugins::Pmd;

using execHelper::test::OptionsStub;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::copyAndAppend;
using execHelper::core::test::ExecutorStub;

namespace {
    const czstring<> pmdConfigKey("pmd");
    const czstring<> PLUGIN_CONFIG_KEY("pmd");
    const czstring<> EXEC_CONFIG_KEY("exec");
    const czstring<> TOOL_CONFIG_KEY("tool");
    const czstring<> MINIMUM_TOKENS_CONFIG_KEY("minimum-tokens");
    const czstring<> FILES_CONFIG_KEY("files");
    const czstring<> LANGUAGE_CONFIG_KEY("language");
} // namespace

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

    SCENARIO("Make combinations of different configurations for the pmd plugin", "[plugins][pmd]") {
        MAKE_COMBINATIONS("Of several settings") {
            const string command("pmd-command");

            const Pattern PATTERN1("PATTERN1", {"pattern1"}, 'p', "--pattern1");
            const Pattern PATTERN2("PATTERN2", {"pattern2a", "pattern2b"}, 'q', "--pattern2");
            const Patterns PATTERNS({PATTERN1, PATTERN2});
            const PatternKeys PATTERN_KEYS({PATTERN1.getKey(), PATTERN2.getKey()});

            OptionsStub options;
            for(const auto& pattern : PATTERNS) {
                options.m_patternsHandler->addPattern(pattern);
            }

            SettingsNode& rootSettings = options.m_settings;
            rootSettings.add({PLUGIN_CONFIG_KEY, "patterns"}, {PATTERN1.getKey(), PATTERN2.getKey()});

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");

            Pmd plugin;
            Task task;

            Task expectedTask;
            TaskCollection exec({"pmd.sh"});
            TaskCollection tool({"tool1"});
            TaskCollection minimumTokens;
            TaskCollection files;
            TaskCollection verbosity;
            TaskCollection language;
            TaskCollection commandLine;

            SettingsNode::SettingsKeys baseSettingsKeys = {PLUGIN_CONFIG_KEY};
            SettingsNode::SettingsKeys otherBaseSettingsKeys = {PLUGIN_CONFIG_KEY, otherCommandKey};

            COMBINATIONS("Toggle between general and specific command settings") {
                baseSettingsKeys.push_back(command);
            }

            rootSettings.add(copyAndAppend(otherBaseSettingsKeys, EXEC_CONFIG_KEY), "other-exec");
            rootSettings.add(copyAndAppend(otherBaseSettingsKeys, TOOL_CONFIG_KEY), "other-tool");

            COMBINATIONS("Switch off verbosity") {
                options.m_verbosity = false;
            }

            COMBINATIONS("Switch on verbosity") {
                options.m_verbosity = true;
                verbosity.emplace_back("-verbose");
            }

            COMBINATIONS("Switch on the cpd tool") {
                tool.clear();
                tool = {"cpd"};
            }

            COMBINATIONS("Switch on minimum tokens") {
                const string minimumTokensValue("100");
                rootSettings.add(copyAndAppend(baseSettingsKeys, MINIMUM_TOKENS_CONFIG_KEY), minimumTokensValue);

                if(tool[0] == "cpd") {
                    minimumTokens.emplace_back("--minimum-tokens");
                    minimumTokens.emplace_back("100");
                }
            }

            COMBINATIONS("Switch on files") {
                const vector<string> filesValue({"file1", "file2", "file*"});
                rootSettings.add(copyAndAppend(baseSettingsKeys, FILES_CONFIG_KEY), filesValue);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, FILES_CONFIG_KEY), "other-file");

                if(tool[0] == "cpd") {
                    for(const auto& file : filesValue) {
                        files.emplace_back("--files");
                        files.emplace_back(file);
                    }
                }
            }

            COMBINATIONS("Add a language") {
                const string languageValue("language1");
                language.emplace_back("--language");
                language.push_back(languageValue);
                rootSettings.add(copyAndAppend(baseSettingsKeys, LANGUAGE_CONFIG_KEY), languageValue);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, LANGUAGE_CONFIG_KEY), "other-language");
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + PATTERN2.getKey() + "}", "{" + PATTERN2.getKey() + "}"};
                rootSettings.add(copyAndAppend(baseSettingsKeys, "command-line"), commandLine);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "command-line"), "--some-command");
            }

            rootSettings.add(copyAndAppend(baseSettingsKeys, EXEC_CONFIG_KEY), exec);
            rootSettings.add(copyAndAppend(baseSettingsKeys, TOOL_CONFIG_KEY), tool);

            expectedTask.append(exec);
            expectedTask.append(tool);
            expectedTask.append(minimumTokens);
            expectedTask.append(files);
            expectedTask.append(language);
            expectedTask.append(verbosity);
            expectedTask.append(commandLine);

            ExecutorStub::TaskQueue expectedTasks;
            for(const std::map<string, string>& target : options.makePatternPermutator(PATTERN_KEYS)) {
                Task replacedExpectedTask = replacePatternCombinations(expectedTask, target);
                expectedTasks.emplace_back(replacedExpectedTask);
            }

            bool returnCode = plugin.apply(command, task, options);
            THEN_CHECK("It should succeed") {
                REQUIRE(returnCode);
            }

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
            }

        }
    }
} // namespace test
} // namespace plugins
} // namespace execHelper
