#include "unittest/catch.h"

#include "core/pattern.h"
#include "plugins/pluginUtils.h"
#include "plugins/clangStaticAnalyzer.h"
#include "plugins/memory.h"

#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::ExecutorInterface;
using execHelper::core::test::ExecutorStub;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::Pattern;

using execHelper::test::utils::addSettings;
using execHelper::test::OptionsStub;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::getAllPatterns;
using execHelper::test::utils::getAllPatternKeys;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::addPatterns;

namespace {
    const string PLUGIN_CONFIG_KEY("clang-static-analyzer");
    const string PLUGIN_COMMAND("scan-build");
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the configuration settings of the clang-static-analyzer plugin", "[plugins][clangStaticAnalyzer]") {
        MAKE_COMBINATIONS("Of several settings") {
            const string command("clang-static-analyzer-command");

            OptionsStub options;

            TargetUtil targetUtil;
            addPatterns(targetUtil.getPatterns(), options);

            CompilerUtil compilerUtil;
            addPatterns(compilerUtil.getPatterns(), options);

            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, command);
            addSettings(rootSettings[PLUGIN_CONFIG_KEY], "patterns", targetUtil.getKeys());
            addSettings(rootSettings[PLUGIN_CONFIG_KEY], "patterns", compilerUtil.getKeys());
            addSettings(rootSettings[PLUGIN_CONFIG_KEY], "build-command", "memory");

            MemoryHandler memory;

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, otherCommandKey);

            ClangStaticAnalyzer plugin;
            Task task;

            Task expectedTask({PLUGIN_COMMAND});
            TaskCollection verbosity;
            TaskCollection commandLine;

            SettingsNode* settings = &(rootSettings[PLUGIN_CONFIG_KEY]);

            COMBINATIONS("Toggle between general and specific command settings") {
                settings = &rootSettings[PLUGIN_CONFIG_KEY][command];
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}", "blaat/{HELLO}/{" + compilerUtil.compiler.getKey() + "}"};
                addSettings(*settings, "command-line", commandLine);
                addSettings(rootSettings[PLUGIN_CONFIG_KEY][otherCommandKey], "command-line", "--some-command");
            }

            COMBINATIONS("Switch off verbosity") {
                options.m_verbosity = false;
            }

            COMBINATIONS("Switch on verbosity") {
                options.m_verbosity = true;
                verbosity.emplace_back("-v");
            }

            expectedTask.append(commandLine);
            expectedTask.append(verbosity);

            bool returnCode = plugin.apply(command, task, options);
            THEN_CHECK("It should succeed") {
                REQUIRE(returnCode == true);
            }

            THEN_CHECK("It called the right commands") {
                const Memory::Memories& memories = memory.getExecutions();
                REQUIRE(memories.size() == 1U);
                REQUIRE(memories[0].command == command);
                REQUIRE(memories[0].task == expectedTask);
            }
        }
    }

    SCENARIO("Testing the invalid configurations", "[plugins][clangStaticAnalyzer]") {
        GIVEN("An empty setup") {
            const string command("clang-static-analyzer-command");

            OptionsStub options;

            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, PLUGIN_CONFIG_KEY, command);

            ClangStaticAnalyzer plugin;
            Task task;

            WHEN("We apply the plugin") {
                bool returnCode = plugin.apply(command, task, options);

                THEN("It should fail") {
                    REQUIRE(returnCode == false);
                }
            }

            WHEN("We add the build-command as a generic config key without a value") {
                addSettings(rootSettings, PLUGIN_CONFIG_KEY, "build-command");

                bool returnCode = plugin.apply(command, task, options);
                THEN("It should fail") {
                    REQUIRE(returnCode == false);
                }
            }

            WHEN("We add the build-command as a specific config key without a value") {
                addSettings(rootSettings[PLUGIN_CONFIG_KEY], command, "build-command");

                bool returnCode = plugin.apply(command, task, options);
                THEN("It should fail") {
                    REQUIRE(returnCode == false);
                }
            }
        }
    }
} } }
