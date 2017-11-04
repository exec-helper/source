#include "config.h"

#include <gsl/gsl>

#include "yaml/yaml.h"

#include "commandLineOptions.h"
#include "optionDescriptions.h"
#include "patternsHandler.h"
#include "variablesMap.h"

using std::string;

using boost::optional;
using gsl::czstring;
using gsl::not_null;

using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::yaml::Yaml;

namespace {
const czstring<> PATTERNS_KEY = "patterns";

bool processPattern(not_null<VariablesMap*> currentSettings,
                    const SettingsNode& newSettings) noexcept {
    for(const auto& key : newSettings.values()) {
        const SettingsNode& parameter = newSettings[key];
        if(!currentSettings->replace(parameter.key(), parameter.values())) {
            return false;
        }
    }
    return true;
}

Patterns processPatterns(const SettingsNode& settings) noexcept {
    Patterns result;
    if(settings.contains(PATTERNS_KEY)) {
        const SettingsNode& patternSettings = settings[PATTERNS_KEY];
        for(const auto& patternKey : patternSettings.values()) {
            VariablesMap newPatternMap =
                PatternsHandler::getDefaultPatternMap(patternKey);
            if(!processPattern(&newPatternMap, patternSettings[patternKey])) {
                continue;
            }
            auto newPattern =
                PatternsHandler::toPattern(patternKey, newPatternMap);
            if(!newPattern) {
                continue;
            }
            result.emplace_back(newPattern.get());
        }
    }
    return result;
}
} // namespace

namespace execHelper {
namespace config {
optional<PatternSettingsPair> parseSettingsFile(const Path& file) noexcept {
    SettingsNode configuration("exec-helper");

    Yaml yaml(file);
    if(!yaml.getTree({}, &configuration)) {
        LOG(error) << "Could not get settings tree";
        return boost::none;
    }

    Patterns patterns = processPatterns(configuration);
    configuration.clear(PATTERNS_KEY);
    return make_pair(patterns, configuration);
}
} // namespace config
} // namespace execHelper
