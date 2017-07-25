#include "commandPlugin.h"

#include <gsl/string_span>

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
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
   const czstring<> PLUGIN_NAME = "commands";
} // namespace

namespace execHelper { namespace plugins {
    std::string CommandPlugin::getPluginName() const noexcept {
        return PLUGIN_NAME;
    }

    VariablesMap CommandPlugin::getVariablesMap(const FleetingOptionsInterface& fleetingOptions) const noexcept {
        VariablesMap defaults(PLUGIN_NAME);
        defaults.add(PLUGIN_NAME, fleetingOptions.getCommands());
        return defaults;
    }

    bool CommandPlugin::apply(Task task, const VariablesMap& variables, const Patterns& patterns) const noexcept {
        ensures(variables.get<CommandCollection>(PLUGIN_NAME) != boost::none);
        auto commands = variables.get<CommandCollection>(PLUGIN_NAME).get();
        ExecutePlugin executePlugin(commands);
        return executePlugin.apply(task, variables, patterns);
    }
} // namespace plugins
} // namespace execHelper
