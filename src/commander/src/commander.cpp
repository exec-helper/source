#include "commander.h"

#include <filesystem>
#include <iostream>
#include <utility>

#include <gsl/pointers>
#include <gsl/string_span>

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

namespace filesystem = std::filesystem;

namespace {
const czstring<> WORKING_DIR_PATTERN_KEY = "EH_WORKING_DIR";

inline Patterns addPredefinedPatterns(Patterns patterns) {
    patterns.emplace_back(Pattern(WORKING_DIR_PATTERN_KEY,
                                  {filesystem::current_path().string()}));
    return patterns;
}
} // namespace

namespace execHelper {
namespace commander {
bool Commander::run(const FleetingOptionsInterface& fleetingOptions,
                    SettingsNode settings, Patterns patterns,
                    const Path& workingDirectory,
                    const EnvironmentCollection& env) noexcept {
    patterns = addPredefinedPatterns(patterns);

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
