#include <string>
#include <vector>

#include "core/execHelperOptions.h"
#include "plugins/bootstrap.h"
#include "plugins/pluginUtils.h"
#include "unittest/catch.h"
#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::string;

using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CommandCollection;

using execHelper::config::SettingsNode;

using execHelper::test::OptionsStub;
using execHelper::core::test::ExecutorStub;
using execHelper::test::utils::copyAndAppend;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::CompilerUtil;
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
            addPatterns(targetUtil.getPatterns(), &options);

            CompilerUtil compilerUtil;
            addPatterns(compilerUtil.getPatterns(), &options);

            SettingsNode& rootSettings = options.m_settings;
            rootSettings.add({pluginConfigKey, "patterns"}, targetUtil.getKeys());
            rootSettings.add({pluginConfigKey, "patterns"}, compilerUtil.getKeys());

            // Add the settings of an other command to make sure we take the expected ones
            const string otherCommandKey("other-command");

            Bootstrap plugin;
            Task task;

            Task expectedTask;

            TaskCollection runCommand = {"./bootstrap.sh"};
            TaskCollection changeDirectoryCommand = {"cd"};
            TaskCollection chainCommandsCommand = {"&&"};
            TaskCollection buildDir;
            TaskCollection verbosity;
            TaskCollection commandLine;

            SettingsNode::SettingsKeys baseSettingsKeys = {pluginConfigKey};
            SettingsNode::SettingsKeys otherBaseSettingsKeys = {pluginConfigKey, otherCommandKey};

            COMBINATIONS("Toggle between general and specific command settings") {
                baseSettingsKeys.push_back(command);
            }

            COMBINATIONS("Set a build directory") {
                const string buildDirValue("{" + compilerUtil.distribution.getKey() + "}/{" + compilerUtil.architecture.getKey() + "}/{HELLO}/{" + compilerUtil.compiler.getKey() + "}/hello{" + compilerUtil.mode.getKey() + " }world"); 
                rootSettings.add(copyAndAppend(baseSettingsKeys, "build-dir"), buildDirValue);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "build-dir"), "some/build/dir");
                buildDir.emplace_back(buildDirValue);
            }

            COMBINATIONS("Set a different file name") {
                const string filename = "other-filename.sh";
                rootSettings.add(copyAndAppend(baseSettingsKeys, "filename"), filename);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "filename"), "other-file.sh");

                runCommand.clear();
                runCommand.emplace_back("./" + filename);
            }

            COMBINATIONS("Set a different absolute file name") {
                const string filename = "/other/absolute/filename.sh";
                rootSettings.add(copyAndAppend(baseSettingsKeys, "filename"), filename);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "filename"), "other-file.sh");

                runCommand.clear();
                runCommand.emplace_back(filename);
            }

            COMBINATIONS("Add a command line") {
                commandLine = {"{" + targetUtil.target.getKey() + "}{" + targetUtil.runTarget.getKey() + "}"};
                rootSettings.add(copyAndAppend(baseSettingsKeys, "command-line"), commandLine);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "command-line"), "--some-command");
            }

            COMBINATIONS("Custom change directory command") {
                const string changeDirectoryCommandValue = "cd{" + compilerUtil.distribution.getKey() + "}";
                rootSettings.add(copyAndAppend(baseSettingsKeys, "change-directory-command"), changeDirectoryCommandValue);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "change-directory-command"), "cdcd");

                changeDirectoryCommand.clear();
                changeDirectoryCommand.push_back(changeDirectoryCommandValue);
            }

            COMBINATIONS("Custom chain commands command") {
                const string chainCommandsCommandValue = "&&{" + compilerUtil.architecture.getKey() + "}";
                rootSettings.add(copyAndAppend(baseSettingsKeys, "chain-commands-command"), chainCommandsCommandValue);
                rootSettings.add(copyAndAppend(otherBaseSettingsKeys, "chain-commands-command"), "&&&&");

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
                REQUIRE(returnCode);
            }

            THEN_CHECK("It called the right commands") {
                REQUIRE(expectedTasks == options.m_executor.getExecutedTasks());
            }
        }
    }
} } }
