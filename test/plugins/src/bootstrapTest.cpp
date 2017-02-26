#include <vector>
#include <string>
#include <fstream>

#include "plugins/bootstrap.h"
#include "plugins/pluginUtils.h"
#include "core/execHelperOptions.h"
#include "utils/utils.h"
#include "unittest/catch.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::vector;
using std::string;
using std::ofstream;

using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CommandCollection;
using execHelper::core::Pattern;
using execHelper::core::PatternsHandler;

using execHelper::config::SettingsNode;
using execHelper::plugins::replacePatternCombinations;

using execHelper::test::OptionsStub;
using execHelper::core::test::ExecutorStub;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::TargetUtilNames;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::CompilerUtilNames;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::addPatterns;

namespace {
    const string pluginConfigKey("bootstrap");
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Test the combination of several settings for the bootstrap plugin", "[plugins][bootstrap]") {
        MAKE_COMBINATIONS("Of several settings") {
            const string command("bootstrap-command");

            OptionsStub options;

            TargetUtil targetUtil;
            addPatterns(targetUtil.getPatterns(), options);

            CompilerUtil compilerUtil;
            addPatterns(compilerUtil.getPatterns(), options);

            SettingsNode& rootSettings = options.m_settings;
            addSettings(rootSettings, pluginConfigKey, command);
            addSettings(rootSettings[pluginConfigKey], "patterns", targetUtil.getKeys());
            addSettings(rootSettings[pluginConfigKey], "patterns", compilerUtil.getKeys());

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");
            addSettings(rootSettings, pluginConfigKey, otherCommandKey);

            Bootstrap plugin;
            Task task;

            Task expectedTask;

            TaskCollection runCommand = {"./bootstrap.sh"};
            TaskCollection changeDirectoryCommand = {"cd"};
            TaskCollection chainCommandsCommand = {"&&"};
            TaskCollection buildDir;
            TaskCollection verbosity;
            TaskCollection commandLine;

            SettingsNode* settings = &(rootSettings[pluginConfigKey]);

            COMBINATIONS("Toggle between general and specific command settings") {
                settings = &rootSettings[pluginConfigKey][command];
            }

            COMBINATIONS("Set a build directory") {
                const string buildDirValue("{" + compilerUtil.distribution.getKey() + "}/{" + compilerUtil.architecture.getKey() + "}/{HELLO}/{" + compilerUtil.compiler.getKey() + "}/hello{" + compilerUtil.mode.getKey() + " }world"); 
                addSettings(*settings, "build-dir", buildDirValue);
                addSettings(rootSettings[pluginConfigKey][otherCommandKey], "build-dir", "some/build/dir");
                buildDir.emplace_back(buildDirValue);
            }

            COMBINATIONS("Set a different file name") {
                const string filename = "other-filename.sh";
                addSettings(*settings, "filename", filename);
                addSettings(rootSettings[pluginConfigKey][otherCommandKey], "filename", "other-file.sh");

                runCommand.clear();
                runCommand.emplace_back("./" + filename);
            }

            COMBINATIONS("Set a different absolute file name") {
                const string filename = "/other/absolute/filename.sh";
                addSettings(*settings, "filename", filename);
                addSettings(rootSettings[pluginConfigKey][otherCommandKey], "filename", "other-file.sh");

                runCommand.clear();
                runCommand.emplace_back(filename);
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}"};
                addSettings(*settings, "command-line", commandLine);
                addSettings(rootSettings[pluginConfigKey][otherCommandKey], "command-line", "--some-command");
            }

            COMBINATIONS("Custom change directory command") {
                const string changeDirectoryCommandValue = "cd{" + compilerUtil.distribution.getKey() + "}";
                addSettings(*settings, "change-directory-command", changeDirectoryCommandValue);
                addSettings(rootSettings[pluginConfigKey][otherCommandKey], "change-directory-command", "cdcd");

                changeDirectoryCommand.clear();
                changeDirectoryCommand.push_back(changeDirectoryCommandValue);
            }

            COMBINATIONS("Custom chain commands command") {
                const string chainCommandsCommandValue = "&&{" + compilerUtil.architecture.getKey() + "}";
                addSettings(*settings, "chain-commands-command", chainCommandsCommandValue);
                addSettings(rootSettings[pluginConfigKey][otherCommandKey], "chain-commands-command", "&&&&");

                chainCommandsCommand.clear();
                chainCommandsCommand.push_back(chainCommandsCommandValue);
            }

            if(! buildDir.empty()) {
                expectedTask.append(changeDirectoryCommand);
                expectedTask.append(buildDir);
                expectedTask.append(chainCommandsCommand);
            }
            expectedTask.append(runCommand);
            expectedTask.append(verbosity);
            expectedTask.append(commandLine);

            ExecutorStub::TaskQueue expectedTasks = getExpectedTasks(expectedTask, compilerUtil, targetUtil);

            bool returnCode = plugin.apply(command, task, options);
            THEN_CHECK("It should succeed") {
                REQUIRE(returnCode == true);
            }

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
            }
        }
    }
} } }
