#include "selector.h"

#include <vector>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/pattern.h"

#include "pluginUtils.h"
#include "executePlugin.h"

using std::string;
using std::vector;

using execHelper::config::SettingsNode;
using execHelper::core::PatternKeys;
using execHelper::core::PatternValues;

namespace execHelper { namespace plugins {
    bool Selector::apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept {
        static const string selectorKey("selector");
        if(! options.getSettings().contains(selectorKey)) {
            return false;
        }
        const SettingsNode& selectorSettings = options.getSettings({"selector"});

        if(! selectorSettings.contains(command)) {
            user_feedback_error("Could not find command '" << command << "' in the " << selectorKey << " settings");
            return false;
        }
        const SettingsNode rootSettings = selectorSettings[command]; 

        static const string patternKey("pattern");
        if(! rootSettings.contains(patternKey)) {
            user_feedback_error("Missing the '" << patternKey << "' keyword in the configuration of " << selectorKey << "[" << command << "] settings");
            return false;
        }
        PatternKeys patternKeysToCheck = rootSettings[patternKey].toStringCollection();
        vector<string> commandsToExecute;
        for(const auto& pattern : options.makePatternPermutator(patternKeysToCheck)) {
            for(const auto& patternValues : pattern) {
                commandsToExecute.push_back(patternValues.second);
            }
        }
        ExecutePlugin executePlugin(commandsToExecute);
        return executePlugin.apply(command, task, options);
    }
} }
