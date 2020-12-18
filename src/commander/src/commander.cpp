#include "commander.h"

#include <filesystem>
#include <iostream>
#include <utility>

#include <gsl/gsl>

#include "config/fleetingOptionsInterface.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/executePlugin.h"

#include "logger.h"

using std::move;

using gsl::czstring;
using gsl::not_null;

using execHelper::config::EnvironmentCollection;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Path;
using execHelper::config::Pattern;
using execHelper::config::Patterns;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;
using execHelper::plugins::ExecutePlugin;
using execHelper::plugins::Plugins;

namespace filesystem = std::filesystem;

namespace {
inline auto addPredefinedPatterns(Patterns patterns, const Path& rootDirectory)
    -> Patterns {
    using namespace std::string_literals;

    const auto WORKING_DIR_PATTERN_KEY = "EH_WORKING_DIR"s;
    const auto ROOT_DIR_PATTERN_KEY = "EH_ROOT_DIR"s;

    patterns.emplace_back(Pattern(WORKING_DIR_PATTERN_KEY,
                                  {filesystem::current_path().string()}));
    patterns.emplace_back(
        Pattern(ROOT_DIR_PATTERN_KEY, {rootDirectory.string()}));
    return patterns;
}
} // namespace

namespace execHelper {
namespace commander {
auto Commander::run(const FleetingOptionsInterface& fleetingOptions,
                    SettingsNode settings, Patterns patterns,
                    const Path& workingDirectory,
                    const EnvironmentCollection& env, Plugins&& plugins,
                    const Path& rootDirectory) -> Tasks {
    patterns = addPredefinedPatterns(patterns, rootDirectory);

    auto foRaii = ExecutePlugin::push(
        not_null<const FleetingOptionsInterface*>(&fleetingOptions));
    auto seRaii = ExecutePlugin::push(move(settings));
    auto paRaii = ExecutePlugin::push(move(patterns));
    auto plRaii = ExecutePlugin::push(move(plugins));
    Task task({}, env, workingDirectory);

    auto commands = fleetingOptions.getCommands();
    if(commands.empty()) {
        throw std::runtime_error("You must define at least one command");
    }
    ExecutePlugin plugin(move(commands));
    auto tasks = plugin.apply(task, VariablesMap("commands"));
    return tasks;
}
} // namespace commander
} // namespace execHelper
