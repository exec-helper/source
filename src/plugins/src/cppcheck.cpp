#include "cppcheck.h"

#include <string>
#include <vector>

#include "config/settingsNode.h"
#include "core/options.h"
#include "core/patterns.h"
#include "core/task.h"
#include "log/log.h"

#include "configValue.h"
#include "pluginUtils.h"

using std::string;

using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;
using execHelper::core::Command;
using execHelper::core::Options;
using execHelper::core::PatternCombinations;
using execHelper::config::SettingsNode;

namespace execHelper { namespace plugins {
    bool Cppcheck::apply(const Command& command, Task task, const Options& options) const noexcept {
        static const string cppcheckCommand("cppcheck");
        static const string cppcheckKey("cppcheck");
        const SettingsNode& rootSettings = options.getSettings(cppcheckKey);  
        task.append(cppcheckCommand);
        task.append(getEnabledChecks(command, rootSettings));

        if(options.getVerbosity()) {
            task.append("--verbose");
        }

        task.append(getCommandLine(command, rootSettings));
        task.append(getSourceDir(command, rootSettings));

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task cppcheckTask = replacePatternCombinations(task, combination);
            registerTask(cppcheckTask, options);
        }
        return true;
    }

    string Cppcheck::getSourceDir(const Command& command, const SettingsNode& rootSettings) noexcept {
        static const string targetDirKey("target-path");

        string sourceDir = ConfigValue<string>::get("src-dir", ".", command, rootSettings);
        string targetPath = ConfigValue<string>::get("target-path", "", command, rootSettings);

        if(targetPath.empty()) {
            return sourceDir;
        }
        return sourceDir + "/" + targetPath;
    }

    string Cppcheck::getEnabledChecks(const Command& command, const SettingsNode& rootSettings) noexcept {
        TaskCollection enabledChecks = ConfigValue<TaskCollection>::get("enable-checks", {"all"}, command, rootSettings);
        string result("--enable=");

        for(size_t i = 0; i < enabledChecks.size() - 1; ++i) {
            result += enabledChecks[i] + ',';
        }
        result += enabledChecks.back();
        return result;
    }
} }
