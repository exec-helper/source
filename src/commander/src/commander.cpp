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
using execHelper::plugins::ExecutePlugin;
using execHelper::plugins::Plugins;

namespace filesystem = std::filesystem;

namespace {
const czstring<> WORKING_DIR_PATTERN_KEY = "EH_WORKING_DIR";

inline auto addPredefinedPatterns(Patterns patterns) -> Patterns {
    patterns.emplace_back(Pattern(WORKING_DIR_PATTERN_KEY,
                                  {filesystem::current_path().string()}));
    return patterns;
}
} // namespace

namespace execHelper {
namespace commander {
auto Commander::run(const FleetingOptionsInterface& fleetingOptions,
                    SettingsNode settings, Patterns patterns,
                    const Path& workingDirectory,
                    const EnvironmentCollection& env,
                    Plugins&& plugins) noexcept -> bool {
    patterns = addPredefinedPatterns(patterns);

    ExecutePlugin::push(
        not_null<const FleetingOptionsInterface*>(&fleetingOptions));
    ExecutePlugin::push(move(settings));
    ExecutePlugin::push(move(patterns));
    ExecutePlugin::push(move(plugins));
    Task task({}, env, workingDirectory);

    auto commands = fleetingOptions.getCommands();
    if(commands.empty()) {
        user_feedback_error("You must define at least one command");
        return false;
    }
    ExecutePlugin plugin(move(commands));
    auto returnCode = plugin.apply(task, VariablesMap("commands"), patterns);

    ExecutePlugin::popPlugins();
    ExecutePlugin::popFleetingOptions();
    ExecutePlugin::popSettingsNode();
    ExecutePlugin::popPatterns();
    return returnCode;
}
} // namespace commander
} // namespace execHelper
