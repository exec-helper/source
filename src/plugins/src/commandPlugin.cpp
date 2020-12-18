#include "commandPlugin.h"

#include <gsl/string_span>
#include <stdexcept>

#include "config/variablesMap.h"
#include "log/assertions.h"

#include "commandLine.h"
#include "executePlugin.h"
#include "logger.h"
#include "pluginUtils.h"

using std::string;

using gsl::czstring;

using execHelper::config::CommandCollection;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::Tasks;

namespace {
const czstring<> PLUGIN_NAME = "commands";
} // namespace

namespace execHelper::plugins {
auto CommandPlugin::getVariablesMap(
    const FleetingOptionsInterface& fleetingOptions) const noexcept
    -> VariablesMap {
    VariablesMap defaults(PLUGIN_NAME);
    if(!defaults.add(PLUGIN_NAME, fleetingOptions.getCommands())) {
        LOG(error) << "Failed to add key '" << PLUGIN_NAME << "'";
    }
    return defaults;
}

auto CommandPlugin::apply(Task task, const VariablesMap& variables) const
    -> Tasks {
    if(!variables.get<CommandCollection>(PLUGIN_NAME)) {
        throw std::runtime_error("Define at least one command to execute");
    }
    auto commands = *(variables.get<CommandCollection>(PLUGIN_NAME));
    ExecutePlugin executePlugin(commands);
    return executePlugin.apply(task, variables);
}

auto CommandPlugin::summary() const noexcept -> std::string {
    return "Command (internal)";
}
} // namespace execHelper::plugins
