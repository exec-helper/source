#include "pluginUtils.h"

#include <filesystem>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "config/environment.h"
#include "config/path.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/patterns.h"

#include "commandLineCommand.h"
#include "logger.h"
#include "luaPlugin.h"
#include "memory.h"
#include "plugin.h"

using namespace std;
namespace filesystem = std::filesystem;

using execHelper::config::Command;
using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Path;
using execHelper::config::Paths;
using execHelper::config::PatternCombinations;
using execHelper::config::PatternKey;
using execHelper::config::PatternKeys;
using execHelper::config::Patterns;
using execHelper::config::PatternValue;
using execHelper::config::PatternValues;
using execHelper::config::SettingsKeys;
using execHelper::config::VariablesMap;
using execHelper::core::replacePatterns;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::plugins::PatternPermutator;

namespace execHelper::plugins {
auto getPatternsKey() noexcept -> const PatternKey& {
    static const PatternKey key("patterns");
    return key;
}

auto makePatternPermutator(const Patterns& patterns) noexcept
    -> PatternPermutator {
    std::map<PatternKey, PatternValues> patternValuesMap;
    if(patterns.empty()) {
        // If no patterns were given, iterate once
        patternValuesMap.emplace("NO-KEY", PatternValues({"NO-VALUE"}));
    }
    for(const auto& pattern : patterns) {
        patternValuesMap.emplace(pattern.getKey(), pattern.getValues());
    }
    return plugins::PatternPermutator(patternValuesMap);
}

auto replacePatternsInEnvironment(
    const EnvironmentCollection&
        env, // NOLINT(bugprone-easily-swappable-parameters)
    const PatternCombinations& patternCombinations) noexcept
    -> EnvironmentCollection {
    EnvironmentCollection replaced;

    for(const auto& keyValue : env) {
        auto key = keyValue.first;
        auto value = keyValue.second;

        for(const auto& pattern : patternCombinations) {
            key = replacePatterns(key, pattern.first, pattern.second);
            value = replacePatterns(value, pattern.first, pattern.second);
        }
        replaced.emplace(std::make_pair(key, value));
    }
    return replaced;
}

auto replacePatternCombinations(
    const Task& task, const PatternCombinations& patternCombinations) noexcept
    -> Task {
    Task replacedTask;
    replacedTask.setEnvironment(replacePatternsInEnvironment(
        task.getEnvironment(), patternCombinations));

    auto newWorkingDir = replacePatternCombinations(
        task.getWorkingDirectory().string(), patternCombinations);
    replacedTask.setWorkingDirectory(move(newWorkingDir));

    for(auto argument : task.getTask()) {
        argument = replacePatternCombinations(argument, patternCombinations);
        replacedTask.append(move(argument));
    }
    return replacedTask;
}

auto replacePatternCombinations(
    std::string element,
    const config::PatternCombinations& patternCombinations) noexcept
    -> std::string {

    for(const auto& pattern : patternCombinations) {
        element = replacePatterns(element, pattern.first, pattern.second);
    }
    return element;
}

auto getEnvironment(const VariablesMap& variables) noexcept
    -> EnvironmentCollection {
    EnvironmentCollection result;
    SettingsKeys key({string(ENVIRONMENT_KEY)});
    auto environmentOpt = variables.get<vector<string>>(key);
    for(auto variableName : environmentOpt.value_or(vector<string>())) {
        auto variableValueOpt =
            variables.get<string>({string(ENVIRONMENT_KEY), variableName});
        if(!variableValueOpt) {
            LOG(warning) << "Environment variable '" << variableName
                         << "' does not have an associated value. Ignoring it.";
            continue;
        }
        result.emplace(variableName, move(variableValueOpt.value()));
    }
    return result;
}

auto getWorkingDirKey() noexcept -> const string& {
    static const string workingDirKey("working-dir");
    return workingDirKey;
}

auto toString(const PatternKeys& values) noexcept -> string {
    string result;
    if(values.empty()) {
        return result;
    }
    result.append(values.front());
    for(auto it = values.begin() + 1; it != values.end(); ++it) {
        result.append(", ").append(*it);
    }
    return result;
}

namespace detail {
auto discoverLuaPlugins(
    const Paths& searchPaths,
    const std::function<void(const filesystem::path& plugin)>&
        callback) noexcept {
    /**
     * We search the searchpaths in reverse and overwrite plugins with the same name in later search paths
     */
    LOG(debug) << "Discovering lua plugins...";
    for(const auto& path : searchPaths) {
        LOG(trace) << "Discovering plugins for path " << path;
        try {
            for(const auto& entry : filesystem::directory_iterator(path)) {
                if(entry.is_regular_file() &&
                   entry.path().extension() == ".lua") {
                    LOG(trace) << "Module " << entry.path().stem()
                               << " found at " << path;
                    auto newId = entry.path().stem().string();
                    callback(entry.path());
                }
            }
        } catch(const filesystem::filesystem_error& e) {
            user_feedback_error("Failed to discover plugins for path "
                                << path << ". Skipping it");
            LOG(warning) << "Failed to discover plugins for path " << path
                         << ": " << e.what();
        }
    }
}
} // namespace detail

/**
 * Discover all compatible plugins in the given search paths. This function does *not* recursively seek in these paths.
 *
 * \param[in] searchPaths   The search paths from the lowest priority to the hightest (collisions of plugins in later paths overwrite the ones from earlier ones)
 * \returns     A mapping of the discovered plugins
 */
auto discoverPlugins(const Paths& searchPaths) noexcept -> Plugins {
    Plugins plugins{{"command-line-command", &commandLineCommand}};

    /**
     * We search the searchpaths in reverse and overwrite plugins with the same name in later search paths
     */
    LOG(debug) << "Discovering plugins...";
    detail::discoverLuaPlugins(
        searchPaths, [&plugins](const filesystem::path& path) {
            LOG(trace) << "Module " << path.stem() << " found at " << path;
            auto newId = path.stem().string();
            auto plugin = [path](Task task, const VariablesMap& variables,
                                 const ExecutionContext& context) {
                return luaPlugin(move(task), variables, context, path);
            };

            if(plugins.count(newId) == 0) {
                plugins.try_emplace(newId, plugin);
            } else {
                plugins[newId] = plugin;
            }
        });
    return plugins;
}

/**
 * Discover all compatible plugins in the given search paths. This function does *not* recursively seek in these paths.
 *
 * \param[in] searchPaths   The search paths from the lowest priority to the hightest (collisions of plugins in later paths overwrite the ones from earlier ones)
 * \returns     A mapping of the discovered plugins to their summary
 */
auto discoverPluginSummaries(const Paths& searchPaths) noexcept
    -> PluginSummaries {
    PluginSummaries plugins{
        {"command-line-command", string(commandLineCommandSummary())}};

    detail::discoverLuaPlugins(searchPaths,
                               [&plugins](const filesystem::path& path) {
                                   auto newId = path.stem().string();
                                   auto summary = luaPluginSummary(path);

                                   if(plugins.count(newId) == 0) {
                                       plugins.try_emplace(newId, summary);
                                   } else {
                                       plugins[newId] = summary;
                                   }
                               });
    return plugins;
}
} // namespace execHelper::plugins
