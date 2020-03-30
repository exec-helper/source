#include "config.h"

#include <string>
#include <string_view>

#include "yaml/yaml.h"

#include "commandLineOptions.h"
#include "logger.h"
#include "optionDescriptions.h"
#include "pattern.h"
#include "patternsHandler.h"
#include "variablesMap.h"

using std::optional;
using std::string;
using std::string_view;

using execHelper::config::Patterns;
using execHelper::config::PatternsHandler;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::SettingsValues;
using execHelper::config::VariablesMap;
using execHelper::yaml::Yaml;

namespace {
using namespace std::literals;
const string_view patternsKey = "patterns"sv;

auto processPatterns(const SettingsNode& settings) noexcept -> Patterns {
    Patterns result;
    if(settings.contains(string(patternsKey))) {
        const SettingsNode& patternSettings = settings[string(patternsKey)];
        for(const auto& patternKey : settings.get<SettingsValues>(
                string(patternsKey), SettingsValues())) {
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

namespace execHelper::config {
auto parseSettingsFile(const Path& file) noexcept
    -> optional<PatternSettingsPair> {
    SettingsNode configuration("exec-helper");

    Yaml yaml(file);
    if(!yaml.getTree({}, &configuration)) {
        LOG(error) << "Could not get settings tree";
        return std::nullopt;
    }

    Patterns patterns = processPatterns(configuration);
    configuration.clear(string(patternsKey));
    return make_pair(patterns, configuration);
}
} // namespace execHelper::config
