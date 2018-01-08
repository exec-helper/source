#include "selector.h"

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/task.h"

#include "executePlugin.h"
#include "logger.h"
#include "pluginUtils.h"

using std::string;

using gsl::czstring;

using execHelper::config::CommandCollection;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::PatternKeys;
using execHelper::config::PatternValues;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
const czstring<> PLUGIN_NAME = "selector";
const czstring<> PATTERN_KEY = "patterns";
} // namespace

namespace execHelper {
namespace plugins {
string Selector::getPluginName() const noexcept { return PLUGIN_NAME; }

VariablesMap Selector::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept {
    return VariablesMap("selector");
}

bool Selector::apply(Task task, const VariablesMap& variables,
                     const Patterns& patterns) const noexcept {
    auto patternKeys = variables.get<PatternKeys>(PATTERN_KEY);
    if(!patternKeys) {
        user_feedback_error("Missing the '"
                            << PATTERN_KEY
                            << "' keyword in the configuration of the "
                            << PLUGIN_NAME << " settings");
        return false;
    }
    LOG(debug) << "Using the patterns '" << toString(patternKeys.get())
               << "' for selecting";

    CommandCollection commandsToExecute;
    for(const auto& pattern : patterns) {
        auto values = pattern.getValues();
        commandsToExecute.insert(commandsToExecute.end(), values.begin(),
                                 values.end());
    }

    ExecutePlugin executePlugin(commandsToExecute);
    return executePlugin.apply(task, variables, patterns);
}
} // namespace plugins
} // namespace execHelper
