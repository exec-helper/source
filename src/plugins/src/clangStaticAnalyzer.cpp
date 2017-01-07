#include "clangStaticAnalyzer.h"

#include <string>
#include <vector>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"
#include "core/execHelperOptions.h"

#include "pluginUtils.h"
#include "executePlugin.h"

using std::string;
using std::vector;

using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Command;
using execHelper::config::SettingsNode;
using execHelper::plugins::getContainingSettings;

namespace {
    const string clangStaticAnalyzerCommand("scan-build");
}

namespace execHelper { namespace plugins {
    bool ClangStaticAnalyzer::apply(const Command& command, Task& task, const Options& options) const noexcept {
        static string clangStaticAnalyzerKey("clang-static-analyzer");
        const SettingsNode& rootSettings = options.getSettings(clangStaticAnalyzerKey);  
        task.append(clangStaticAnalyzerCommand);
        task.append(getCommandLine(command, rootSettings));

        string cleanCommand = getSystemName(command, "clean-command", rootSettings);
        string buildCommand = getSystemName(command, "build-command", rootSettings);

        ExecutePlugin executePlugin({cleanCommand, buildCommand});
        return executePlugin.apply(command, task, options);
    }

    string ClangStaticAnalyzer::getSystemName(const Command& command, const string& key, const SettingsNode& rootSettings) noexcept {
        const SettingsNode& settings = getContainingSettings(command, rootSettings, key);
        if(!settings.contains(key)) {
            return ""; 
        }
        TaskCollection systemSetting = settings[key].toStringCollection();
        if(systemSetting.empty()) {
            return ""; 
        }
        return systemSetting.back();
    }
} }
