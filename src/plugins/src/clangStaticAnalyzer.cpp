#include "clangStaticAnalyzer.h"

#include <string>

#include "config/settingsNode.h"
#include "core/execHelperOptions.h"
#include "core/patterns.h"
#include "core/task.h"
#include "log/log.h"

#include "configValue.h"
#include "executePlugin.h"

using std::string;

using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Command;
using execHelper::config::SettingsNode;

namespace execHelper { namespace plugins {
    bool ClangStaticAnalyzer::apply(const Command& command, Task& task, const Options& options) const noexcept {
        static const string clangStaticAnalyzerCommand("scan-build");
        static const string clangStaticAnalyzerKey("clang-static-analyzer");

        const SettingsNode& rootSettings = options.getSettings(clangStaticAnalyzerKey);  
        task.append(clangStaticAnalyzerCommand);

        // Note: we leave replacing the values in the command line to the build-command plugin
        task.append(getCommandLine(command, rootSettings));

        TaskCollection buildCommand = getSystemName(command, "build-command", rootSettings);
        if(buildCommand.empty()) {
            user_feedback_error("Missing 'build-command' option for the '" << command << "' command and the clang-static-analyzer plugin");
            return false;
        }

        if(options.getVerbosity()) {
            task.append("-v");
        }

        ExecutePlugin buildExecutePlugin(buildCommand);
        return buildExecutePlugin.apply(command, task, options);
    }

    TaskCollection ClangStaticAnalyzer::getSystemName(const Command& command, const string& key, const SettingsNode& rootSettings) noexcept {
        return ConfigValue<TaskCollection>::get(key, {}, command, rootSettings);
    }
} }
