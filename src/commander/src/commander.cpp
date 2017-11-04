#include "commander.h"

#include <iostream>
#include <utility>

#include "config/fleetingOptionsInterface.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/executePlugin.h"

#include "logger.h"

using std::move;

using gsl::not_null;

using execHelper::config::EnvironmentCollection;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Path;
using execHelper::config::Patterns;
using execHelper::config::SettingsNode;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::ExecutePlugin;

namespace execHelper {
namespace commander {
bool Commander::run(const FleetingOptionsInterface& fleetingOptions,
                    SettingsNode&& settings, Patterns&& patterns,
                    const Path& workingDirectory,
                    const EnvironmentCollection& env) noexcept {
    ExecutePlugin::push(
        not_null<const FleetingOptionsInterface*>(&fleetingOptions));
    ExecutePlugin::push(move(settings));
    ExecutePlugin::push(move(patterns));
    Task task;
    task.setWorkingDirectory(workingDirectory);
    task.setEnvironment(env);

    auto commands = fleetingOptions.getCommands();
    if(commands.empty()) {
        user_feedback_error("You must define at least one command");
        return false;
    }
    ExecutePlugin plugin(move(commands));
    auto returnCode = plugin.apply(task, VariablesMap("commands"), patterns);

    ExecutePlugin::popFleetingOptions();
    ExecutePlugin::popSettingsNode();
    ExecutePlugin::popPatterns();
    return returnCode;
}
} // namespace commander
} // namespace execHelper
