#include "pmd.h"

#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/settingsNode.h"
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

namespace execHelper { namespace plugins {
    string Pmd::getPluginName() const noexcept {
        return PLUGIN_NAME;
    }

    VariablesMap Pmd::getVariablesMap(const FleetingOptionsInterface& fleetingOptions) const noexcept {
        VariablesMap defaults(PLUGIN_NAME);
        defaults.add(EXEC_KEY, PLUGIN_NAME);
        defaults.add(TOOL_KEY, "cpd");
        defaults.add(COMMAND_LINE_KEY);
        const string verbosity = fleetingOptions.getVerbosity() ? "yes" : "no";
        defaults.add(VERBOSITY_KEY, verbosity);
        return defaults;
    }

    bool Pmd::apply(Task task, const VariablesMap& variables, const Patterns& patterns) const noexcept {
        task.append(variables.get<string>(EXEC_KEY).get());
        auto tool = variables.get<string>(TOOL_KEY).get();
        task.append(tool);

        auto language = variables.get<string>(LANGUAGE_KEY);
        if(language) {
            task.append({"--language", language.get()});
        }

        if(variables.get<string>(VERBOSITY_KEY).get() == "yes") {
            task.append("-verbose");
        }

        if(tool == "cpd") {
            auto minimumTokens = variables.get<MinimumTokens>(MINIMUM_TOKENS_KEY);
            if(minimumTokens) {
                task.append({"--minimum-tokens", minimumTokens.get()});
            }

            auto files = variables.get<Files>(FILES_KEY);
            if(files) {
                for(const auto& file : files.get()) {
                    task.append({"--files", file});
                } 
            }
        }
        task.append(variables.get<CommandLineArgs>(COMMAND_LINE_KEY).get());

        for(const auto& combination : makePatternPermutator(patterns)) {
            Task newTask = replacePatternCombinations(task, combination);
            if(! registerTask(newTask)) {
                return false;
            }
        }
        return true;
    }
} // namespace plugins
} // namespace execHelper
