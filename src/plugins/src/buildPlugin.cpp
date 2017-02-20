#include "buildPlugin.h"

#include <string>

#include "config/settingsNode.h"
#include "core/patterns.h"
#include "core/options.h"

#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Command;
using execHelper::core::replacePatterns;
using execHelper::core::PatternCombinations;

namespace execHelper { namespace plugins {
    const std::string& BuildPlugin::getBuildDirKey() noexcept {
        static const string buildDirKey("build-dir");
        return buildDirKey;
    }

    bool BuildPlugin::getMultiThreaded(const std::string& command, const SettingsNode& rootSettings, const Options& options) noexcept {
        static const string singleThreadedKey("single-threaded");

        if(options.getSingleThreaded()) {
            return false;
        }

        const SettingsNode settings = getContainingSettings(command, rootSettings, singleThreadedKey); 
        if(settings.contains(singleThreadedKey)) {
            TaskCollection commandArguments = settings[singleThreadedKey].toStringCollection();
            if(commandArguments.size() > 0 && commandArguments[0] == "yes") {
                return false;
            }
        }
        return true;
    }
} }

