#include "buildPlugin.h"

#include <string>

#include "config/settingsNode.h"
#include "core/options.h"
#include "core/patterns.h"

#include "configValue.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Command;
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

        const string singleThreaded = ConfigValue<string>::get("single-threaded", "no", command, rootSettings);
        return singleThreaded != "yes";
    }
} // namespace plugins
} // namespace execHelper
