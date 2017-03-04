#include "selector.h"

#include <vector>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/pattern.h"

#include "configValue.h"
#include "executePlugin.h"

using std::string;
using std::vector;

using execHelper::config::SettingsNode;
using execHelper::core::PatternKeys;
using execHelper::core::PatternValues;
using execHelper::core::TaskCollection;

namespace execHelper { namespace plugins {
    bool Selector::apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept {
        static const string selectorKey("selector");
        if(! options.getSettings().contains(selectorKey)) {
            return false;
        }
        const SettingsNode& rootSettings = options.getSettings({"selector"});
        boost::optional<TaskCollection> patternSettings = ConfigValue<TaskCollection>::getSetting(getPatternsKey(), rootSettings, {command});
        if(patternSettings == boost::none) {
            user_feedback_error("Missing the '" << getPatternsKey() << "' keyword in the configuration of " << selectorKey << "[" << command << "] settings");
            return false;
        }
        PatternKeys patternKeysToCheck = patternSettings.get();
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
