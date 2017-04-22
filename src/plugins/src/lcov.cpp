#include "lcov.h"

#include <string>

#include "config/settingsNode.h"
#include "core/task.h"
#include "log/log.h"

#include "configValue.h"
#include "executePlugin.h"
#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Options;
using execHelper::core::Task;
using execHelper::core::Command;
using execHelper::core::TaskCollection;
using execHelper::core::PatternCombinations;

using execHelper::plugins::replacePatternCombinations;
using execHelper::plugins::registerTask;

namespace {
    const char* lcovBinary("lcov");
    const char* baseDirectoryOption("--base-directory");
    const char* directoryOption("--directory");

    void runTask(const Task& task, const Options& options, const PatternCombinations& combination) {
        replacePatternCombinations(task, combination);
        registerTask(task, options);
    }
}

namespace execHelper { namespace plugins {
    bool Lcov::apply(const Command& command, Task& task, const Options& options) const noexcept {
        static const string lcovConfigKey("lcov");

        const SettingsNode& rootSettings = options.getSettings(lcovConfigKey);

        static string runCommandConfigKey("run-command");
        boost::optional<string> runCommandValue = ConfigValue<string>::getSetting(runCommandConfigKey, rootSettings, command);
        if(runCommandValue == boost::none) {
            user_feedback_error("Could not find the '" << runCommandConfigKey << "' setting for command '" << command << "' in the '" << lcovConfigKey << "' settings");
            return false;
        }
        const string runCommand = runCommandValue.get();

        const string infoFile = ConfigValue<string>::get("info-file", "lcov-plugin.info", command, rootSettings);
        const string baseDirectory = ConfigValue<string>::get("base-directory", ".", command, rootSettings);
        const string directory = ConfigValue<string>::get("directory", ".", command, rootSettings);
        const TaskCollection commandLine = ConfigValue<TaskCollection>::get("command-line", {}, command, rootSettings);

        Task zeroCountersTask = task;
        const bool zeroCounters = generateZeroCountersTask(command, rootSettings, baseDirectory, directory, commandLine, zeroCountersTask);

        Task genHtmlTask = task;
        const bool genHtml = generateGenHtmlTask(command, rootSettings, infoFile, genHtmlTask);

        Task captureTask = task;
        generateCaptureTask(baseDirectory, directory, infoFile, commandLine, captureTask);

        Task excludeTask = task;
        bool exclude = generateExcludeTask(command, rootSettings, infoFile, commandLine, excludeTask);

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            if(zeroCounters) {
                runTask(zeroCountersTask, options, combination);
            }

            ExecutePlugin execute({runCommand});
            if(! execute.apply(command, task, options)) {
                return false;
            }

            runTask(captureTask, options, combination);
            if(exclude) {
                runTask(excludeTask, options, combination);
            }
            if(genHtml) {
                runTask(genHtmlTask, options, combination);
            }
        }
        return true;
    }

    inline bool Lcov::generateGenHtmlTask(const Command& command, const SettingsNode& rootSettings, const string& infoFile, Task& task) noexcept {
        const string genHtmlOutput = ConfigValue<string>::get("gen-html-output", ".", command, rootSettings);
        const string genHtmlTitle = ConfigValue<string>::get("gen-html-title", "Hello", command, rootSettings);
        const TaskCollection genHtmlCommandLine = ConfigValue<TaskCollection>::get("gen-html-command-line", {}, command, rootSettings);

        const string genHtmlValue = ConfigValue<string>::get("gen-html", "no", command, rootSettings);
        if(genHtmlValue == "yes") {
            task.append(TaskCollection({"genhtml", "--output-directory", genHtmlOutput, "--title", genHtmlTitle, infoFile}));
            task.append(genHtmlCommandLine);
            return true;
        }
        return false;
    }

    inline bool Lcov::generateZeroCountersTask(const Command& command, const SettingsNode& rootSettings, const string& baseDirectory, const string& directory, const TaskCollection& commandLine, Task& task) noexcept {
        const string zeroCountersValue = ConfigValue<string>::get("zero-counters", "no", command, rootSettings);
        if(zeroCountersValue == "yes") {
            task = task;
            task.append(TaskCollection({lcovBinary, baseDirectoryOption, baseDirectory, directoryOption, directory, "--zerocounters"}));
            task.append(commandLine);
            return true;
        }
        return false;
    }

    inline TaskCollection Lcov::getExcludes(const Command& command, const SettingsNode& rootSettings) noexcept {
        TaskCollection excludes = ConfigValue<TaskCollection>::get("excludes", {}, command, rootSettings);
        for(auto& exclude : excludes) {
            exclude.insert(0, R"(")");
            exclude.append(R"(")");
        }
        return excludes;
    }

    inline bool Lcov::generateExcludeTask(const Command& command, const SettingsNode& rootSettings, const string& infoFile, const TaskCollection& commandLine, Task& task) noexcept {
        const TaskCollection excludes = getExcludes(command, rootSettings);
        if(excludes.empty()) {
            return false;
        }

        task.append(TaskCollection({lcovBinary}));
        task.append(TaskCollection({"--remove", infoFile}));
        task.append(excludes);
        task.append(TaskCollection({"--output-file", infoFile}));
        task.append(commandLine);
        return true;
    }

    inline void Lcov::generateCaptureTask(const string& baseDirectory, const string& directory, const string& infoFile, const TaskCollection& commandLine, Task& task) noexcept {
        task.append(TaskCollection({lcovBinary, baseDirectoryOption, baseDirectory, directoryOption, directory, "--capture", "--output", infoFile}));
        task.append(commandLine);
    }
} }
