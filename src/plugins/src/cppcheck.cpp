#include "cppcheck.h"

#include <string>
#include <vector>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"

#include "pluginUtils.h"

using std::string;
using std::vector;

using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Patterns;
using execHelper::core::Command;
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
        task.append(getCommandLine(command, rootSettings));

        task.append(getSourceDir(command, rootSettings));
        registerTask(task, options);
        return true;
    }

    TaskCollection Cppcheck::getSourceDir(const Command& command, const SettingsNode& rootSettings) noexcept {
        static const string sourceDirKey("src-dir");

        TaskCollection result;
        const SettingsNode settings = getContainingSettings(command, rootSettings, sourceDirKey); 
        if(! settings.contains(sourceDirKey)) {
            result.push_back(".");
            return result;
        }
        return settings[sourceDirKey].toStringCollection();
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
