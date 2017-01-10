#include "cppcheck.h"

#include <string>
#include <vector>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"
#include "core/execHelperOptions.h"

#include "pluginUtils.h"

using std::string;
using std::vector;

using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;
using execHelper::core::Command;
using execHelper::core::ExecHelperOptions;
using execHelper::core::PatternCombinations;
using execHelper::config::SettingsNode;

namespace {
    const string cppcheckCommand("cppcheck");
}

namespace execHelper { namespace plugins {
    bool Cppcheck::apply(const Command& command, Task& task, const Options& options) const noexcept {
        static string cppcheckKey("cppcheck");
        const SettingsNode& rootSettings = options.getSettings(cppcheckKey);  
        task.append(cppcheckCommand);
        task.append(getEnabledChecks(command, rootSettings));

        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
        PatternKeys patterns; 
        if(patternSettings.contains(getPatternsKey())) {
            patterns = patternSettings[getPatternsKey()].toStringCollection();
        }
        for(const auto& combination : options.makePatternPermutator(patterns)) {
            Task cppcheckTargetTask = task;
            cppcheckTargetTask.append(getCommandLine(command, rootSettings, combination));
            cppcheckTargetTask.append(getSourceDir(command, rootSettings, combination));
            registerTask(cppcheckTargetTask, options);
        }
        return true;
    }

    TaskCollection Cppcheck::getSourceDir(const Command& command, const SettingsNode& rootSettings, const PatternCombinations& /*patternCombinations*/) noexcept {
        static const string sourceDirKey("src-dir");
        static const string targetDirKey("target-path");

        string sourceDir;
        const SettingsNode sourceSettings = getContainingSettings(command, rootSettings, sourceDirKey); 
        if(! sourceSettings.contains(sourceDirKey)) {
            sourceDir += ".";
        } else {
            TaskCollection sourceDirSettings = sourceSettings[sourceDirKey].toStringCollection();
            sourceDir += sourceDirSettings.back();
        }
        const SettingsNode targetSettings = getContainingSettings(command, rootSettings, targetDirKey); 
        //if(target.getTarget() != "all") {
            //const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
            //Patterns patterns = patternSettings[getPatternsKey()].toStringCollection();
            //sourceDir += "/" + replacePatterns(targetSettings[targetDirKey].toStringCollection().back(), patterns, target);
        //}
        return TaskCollection({sourceDir});
    }

    TaskCollection Cppcheck::getEnabledChecks(const Command& command, const SettingsNode& rootSettings) noexcept {
        static const string enabledChecksKey("enable-checks");
        string enabledChecksOption("--enable=");

        TaskCollection result;
        const SettingsNode settings = getContainingSettings(command, rootSettings, enabledChecksKey); 
        if(! settings.contains(enabledChecksKey)) {
            enabledChecksOption += "all";
            result.push_back(enabledChecksOption);
            return result;
        }

        vector<string> enabledChecks = settings[enabledChecksKey].toStringCollection();
        if(enabledChecks.empty()) {
            enabledChecksOption += "all";
            result.push_back(enabledChecksOption);
            return result;
        }

        enabledChecksOption += enabledChecks[0];
        for(size_t i = 1; i < enabledChecks.size(); ++i) {
            enabledChecksOption +=  "," + enabledChecks[i];
        }
        result.push_back(enabledChecksOption);
        return result;
    }
} }
