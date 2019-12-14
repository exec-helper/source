#include "pmd.h"

#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/patterns.h"
#include "core/task.h"

#include "commandLine.h"
#include "logger.h"
#include "pluginUtils.h"
#include "verbosity.h"

using std::string;
using std::vector;

using gsl::czstring;

using execHelper::config::Command;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::PatternKeys;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
const czstring<> PLUGIN_NAME = "pmd";
const czstring<> EXEC_KEY = "exec";
const czstring<> TOOL_KEY = "tool";
using MinimumTokens = string;
const czstring<> MINIMUM_TOKENS_KEY = "minimum-tokens";
using Files = vector<string>;
const czstring<> FILES_KEY = "files";
const czstring<> LANGUAGE_KEY = "language";
} // namespace

namespace execHelper::plugins {
auto Pmd::getPluginName() const noexcept -> string { return PLUGIN_NAME; }

auto Pmd::getVariablesMap(const FleetingOptionsInterface& fleetingOptions) const
    noexcept -> VariablesMap {
    VariablesMap defaults(PLUGIN_NAME);
    if(!defaults.add(EXEC_KEY, PLUGIN_NAME)) {
        LOG(error) << "Failed to add key '" << EXEC_KEY << "'";
    }
    if(!defaults.add(TOOL_KEY, "cpd")) {
        LOG(error) << "Failed to add key '" << TOOL_KEY << "'";
    }
    if(!defaults.add(COMMAND_LINE_KEY, CommandLineArgs())) {
        LOG(error) << "Failed to add key '" << COMMAND_LINE_KEY << "'";
    }
    const string verbosity = fleetingOptions.getVerbosity() ? "yes" : "no";
    if(!defaults.add(VERBOSITY_KEY, verbosity)) {
        LOG(error) << "Failed to add key '" << VERBOSITY_KEY << "'";
    }
    return defaults;
}

auto Pmd::apply(Task task, const VariablesMap& variables,
                const Patterns& patterns) const noexcept -> bool {
    auto binaryName = *(variables.get<string>(EXEC_KEY));
    auto tool = *(variables.get<string>(TOOL_KEY));
    binaryName.append("-").append(tool);
    task.append(binaryName);

    auto language = variables.get<string>(LANGUAGE_KEY);
    if(language) {
        task.append({"--language", *language});
    }

    if(*(variables.get<string>(VERBOSITY_KEY)) == "yes") {
        task.append("-verbose");
    }

    if(tool == "cpd") {
        auto minimumTokens = variables.get<MinimumTokens>(MINIMUM_TOKENS_KEY);
        if(minimumTokens) {
            task.append({"--minimum-tokens", *minimumTokens});
        }

        auto files = variables.get<Files>(FILES_KEY);
        if(files) {
            for(const auto& file : *files) {
                task.append({"--files", file});
            }
        }
    }
    task.append(*(variables.get<CommandLineArgs>(COMMAND_LINE_KEY)));

    for(const auto& combination : makePatternPermutator(patterns)) {
        Task newTask = replacePatternCombinations(task, combination);
        if(!registerTask(newTask)) {
            return false;
        }
    }
    return true;
}
} // namespace execHelper::plugins
