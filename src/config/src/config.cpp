#include "config.h"

#include <gsl/gsl>

#include "yaml/yaml.h"

#include "commandLineOptions.h"
#include "optionDescriptions.h"
#include "pattern.h"
#include "patternsHandler.h"
#include "variablesMap.h"

using std::optional;
using std::string;

using gsl::czstring;

using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::SettingsValues;
using execHelper::config::VariablesMap;
using execHelper::yaml::Yaml;

namespace {
const czstring<> PATTERNS_KEY = "patterns";

Patterns processPatterns(const SettingsNode& settings) noexcept {
    Patterns result;
    if(settings.contains(PATTERNS_KEY)) {
        const SettingsNode& patternSettings = settings[PATTERNS_KEY];
        for(const auto& patternKey :
            settings.get<SettingsValues>(PATTERNS_KEY, SettingsValues())) {
            VariablesMap newPatternMap =
                PatternsHandler::getDefaultPatternMap(patternKey);
            newPatternMap.overwrite(patternSettings[patternKey]);
            auto newPattern =
                PatternsHandler::toPattern(patternKey, newPatternMap);
            if(!newPattern) {
                continue;
            }
            result.emplace_back(newPattern.value());
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
        return std::nullopt;
    }

    Patterns patterns = processPatterns(configuration);
    configuration.clear(PATTERNS_KEY);
    return make_pair(patterns, configuration);
}
} // namespace config
} // namespace execHelper
