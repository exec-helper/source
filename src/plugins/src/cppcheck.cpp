#include "cppcheck.h"

#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/patterns.h"
#include "core/task.h"
#include "log/assertions.h"

#include "commandLine.h"
#include "logger.h"
#include "pluginUtils.h"
#include "threadedness.h"
#include "verbosity.h"

using std::string;
using std::to_string;
using std::vector;

using gsl::czstring;

using execHelper::config::FleetingOptionsInterface;
using execHelper::config::PatternCombinations;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;

namespace {
const czstring<> PLUGIN_NAME = "cppcheck";
using EnableChecks = vector<string>;
const czstring<> ENABLE_CHECKS_KEY = "enable-checks";
using SourceDir = vector<string>;
const czstring<> SRC_DIR_KEY = "src-dir";
} // namespace

namespace execHelper {
namespace plugins {

std::string Cppcheck::getPluginName() const noexcept { return PLUGIN_NAME; }

VariablesMap
Cppcheck::getVariablesMap(const FleetingOptionsInterface& fleetingOptions) const
    noexcept {
    VariablesMap defaults(PLUGIN_NAME);
    defaults.add(ENABLE_CHECKS_KEY, "all");
    defaults.add(SRC_DIR_KEY, ".");
    defaults.add(COMMAND_LINE_KEY, CommandLineArgs());
    const auto verbosity = fleetingOptions.getVerbosity() ? "yes" : "no";
    defaults.add(VERBOSITY_KEY, verbosity);
    defaults.add(JOBS_KEY, to_string(fleetingOptions.getJobs()));
    return defaults;
}

bool Cppcheck::apply(Task task, const VariablesMap& variables,
                     const Patterns& patterns) const noexcept {
    task.append(PLUGIN_NAME);

    task.append(getEnabledChecks(variables));

    ensures(variables.get<Verbosity>(VERBOSITY_KEY) != boost::none);
    if(variables.get<Verbosity>(VERBOSITY_KEY).get()) {
        task.append("--verbose");
    }

    ensures(variables.get<Jobs>(JOBS_KEY) != boost::none);
    task.append({"-j", to_string(variables.get<Jobs>(JOBS_KEY).get())});

    ensures(variables.get<CommandLineArgs>(COMMAND_LINE_KEY) != boost::none);
    task.append(variables.get<CommandLineArgs>(COMMAND_LINE_KEY).get());

    ensures(variables.get<SourceDir>(SRC_DIR_KEY) != boost::none);
    task.append(variables.get<SourceDir>(SRC_DIR_KEY).get());

    for(const auto& combination : makePatternPermutator(patterns)) {
        Task newTask = replacePatternCombinations(task, combination);
        if(!registerTask(newTask)) {
            return false;
        }
    }
    return true;
}

string Cppcheck::getEnabledChecks(const VariablesMap& variables) noexcept {
    ensures(variables.get<EnableChecks>(ENABLE_CHECKS_KEY) != boost::none);
    auto enabledChecks = variables.get<EnableChecks>(ENABLE_CHECKS_KEY).get();

    string result("--enable=");
    for(size_t i = 0; i < enabledChecks.size() - 1; ++i) {
        result += enabledChecks[i] + ',';
    }
    result += enabledChecks.back();
    return result;
}
} // namespace plugins
} // namespace execHelper
