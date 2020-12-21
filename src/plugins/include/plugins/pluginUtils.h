#ifndef __PLUGIN_UTILS_H__
#define __PLUGIN_UTILS_H__

#include <ostream>
#include <string>

#include "config/commandLineOptions.h"
#include "config/path.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/mapPermutator.h"
#include "core/task.h"

namespace execHelper::plugins {
class Plugin;
using Plugins = std::map<std::string, std::shared_ptr<const Plugin>>;

using PatternPermutator =
    core::MapPermutator<config::PatternKey, config::PatternValue>;

const config::PatternKey& getPatternsKey() noexcept;
const std::string& getWorkingDirKey() noexcept;

config::EnvironmentCollection
getEnvironment(const config::VariablesMap& variables) noexcept;

PatternPermutator
makePatternPermutator(const config::Patterns& patterns) noexcept;

config::EnvironmentCollection replacePatternsInEnvironment(
    const config::EnvironmentCollection& env,
    const config::PatternCombinations& patternCombinations) noexcept;

core::Task replacePatternCombinations(
    const core::Task& task,
    const config::PatternCombinations& patternCombinations) noexcept;

auto replacePatternCombinations(
    std::string element,
    const config::PatternCombinations& patternCombinations) noexcept
    -> std::string;
auto toString(const config::PatternKeys& values) noexcept -> std::string;

auto discoverPlugins(const config::Paths& searchPaths) noexcept -> Plugins;
} // namespace execHelper::plugins

#endif /* __PLUGIN_UTILS_H__ */
