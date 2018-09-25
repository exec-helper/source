#include "lcov.h"

#include <string>

#include <gsl/string_span>

#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "log/assertions.h"

#include "executePlugin.h"
#include "logger.h"
#include "pluginUtils.h"

using std::string;

using gsl::czstring;

using execHelper::config::CommandCollection;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Path;
using execHelper::config::PatternCombinations;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::registerTask;
using execHelper::plugins::replacePatternCombinations;

namespace {
const czstring<> PLUGIN_NAME = "lcov";
using RunCommand = CommandCollection;
const czstring<> RUN_COMMAND = "run-command";
const czstring<> INFO_FILE_KEY = "info-file";
const czstring<> BASE_DIR_KEY = "base-directory";
const czstring<> DIR_KEY = "directory";
using ZeroCounters = bool;
const czstring<> ZERO_COUNTERS_KEY = "zero-counters";
using GenHtml = bool;
const czstring<> GEN_HTML_KEY = "gen-html";
using GenHtmlOutput = Path;
const czstring<> GEN_HTML_OUTPUT_KEY = "gen-html-output";
using GenHtmlTitle = string;
const czstring<> GEN_HTML_TITLE_KEY = "gen-html-title";
using GenHtmlCommandLine = execHelper::plugins::CommandLineArgs;
const czstring<> GEN_HTML_COMMAND_LINE_KEY = "gen-html-command-line";
const czstring<> EXCLUDES_KEY = "excludes";

void runTask(const Task& task, const PatternCombinations& combination) {
    Task replacedTask = replacePatternCombinations(task, combination);
    registerTask(replacedTask);
}
} // namespace

namespace execHelper {
namespace plugins {
string Lcov::getPluginName() const noexcept { return PLUGIN_NAME; }

VariablesMap
Lcov::getVariablesMap(const FleetingOptionsInterface& /*fleetingOptions*/) const
    noexcept {
    VariablesMap defaults(PLUGIN_NAME);
    defaults.add(COMMAND_LINE_KEY, CommandLineArgs());
    defaults.add(INFO_FILE_KEY, "lcov-plugin.info");
    defaults.add(BASE_DIR_KEY, ".");
    defaults.add(DIR_KEY, ".");
    defaults.add(ZERO_COUNTERS_KEY, "no");
    defaults.add(GEN_HTML_KEY, "no");
    defaults.add(GEN_HTML_OUTPUT_KEY, ".");
    defaults.add(GEN_HTML_TITLE_KEY, "Hello");
    defaults.add(GEN_HTML_COMMAND_LINE_KEY, GenHtmlCommandLine());
    defaults.add(EXCLUDES_KEY, Excludes());
    return defaults;
}

bool Lcov::apply(Task task, const VariablesMap& variables,
                 const Patterns& patterns) const noexcept {
    auto runCommandOpt = variables.get<RunCommand>(RUN_COMMAND);
    if(runCommandOpt == std::nullopt) {
        user_feedback_error("Could not find the '"
                            << RUN_COMMAND << "' setting in the '"
                            << PLUGIN_NAME << "' settings");
        return false;
    }
    auto runCommands = runCommandOpt.value();

    if(runCommands.empty()) {
        user_feedback_error("The '" << RUN_COMMAND << "' list is empty");
        return false;
    }

    ensures(variables.get<InfoFile>(INFO_FILE_KEY) != std::nullopt);
    auto infoFile = variables.get<InfoFile>(INFO_FILE_KEY).value();

    ensures(variables.get<BaseDir>(BASE_DIR_KEY) != std::nullopt);
    auto baseDirectory = variables.get<BaseDir>(BASE_DIR_KEY).value();

    ensures(variables.get<Dir>(DIR_KEY) != std::nullopt);
    auto directory = variables.get<Dir>(DIR_KEY).value();

    auto commandLine = variables.get<CommandLineArgs>(COMMAND_LINE_KEY).value();

    bool zeroCounters = variables.get<ZeroCounters>(ZERO_COUNTERS_KEY).value();
    Task zeroCountersTask;
    if(zeroCounters) {
        zeroCountersTask = generateZeroCountersTask(baseDirectory, directory,
                                                    commandLine, task);
    }

    bool genHtml = variables.get<GenHtml>(GEN_HTML_KEY).value();
    Task genHtmlTask;
    if(genHtml) {
        genHtmlTask = generateGenHtmlTask(infoFile, variables, task);
    }

    Task captureTask = generateCaptureTask(baseDirectory, directory, infoFile,
                                           commandLine, task);

    auto exclude = variables.get<Excludes>(EXCLUDES_KEY).value();
    Task excludeTask;
    if(!exclude.empty()) {
        excludeTask =
            generateExcludeTask(variables, infoFile, commandLine, task);
    }

    for(const auto& combination : makePatternPermutator(patterns)) {
        if(zeroCounters) {
            runTask(zeroCountersTask, combination);
        }

        ExecutePlugin execute(runCommands);
        if(!execute.apply(task, variables, patterns)) {
            return false;
        }

        runTask(captureTask, combination);
        if(!exclude.empty()) {
            runTask(excludeTask, combination);
        }
        if(genHtml) {
            runTask(genHtmlTask, combination);
        }
    }
    return true;
}

inline Task Lcov::generateGenHtmlTask(const InfoFile& infoFile,
                                      const VariablesMap& variables,
                                      const Task& task) noexcept {
    Task result = task;
    result.append("genhtml");

    ensures(variables.get<GenHtmlOutput>(GEN_HTML_OUTPUT_KEY) != std::nullopt);
    result.append(
        {"--output-directory",
         variables.get<GenHtmlOutput>(GEN_HTML_OUTPUT_KEY).value().native()});

    ensures(variables.get<GenHtmlTitle>(GEN_HTML_TITLE_KEY) != std::nullopt);
    result.append(
        {"--title", variables.get<GenHtmlTitle>(GEN_HTML_TITLE_KEY).value()});

    ensures(variables.get<GenHtmlCommandLine>(GEN_HTML_COMMAND_LINE_KEY) !=
            std::nullopt);
    result.append(
        variables.get<GenHtmlCommandLine>(GEN_HTML_COMMAND_LINE_KEY).value());

    result.append(infoFile.native());

    return result;
}

inline Task Lcov::generateZeroCountersTask(const BaseDir& baseDirectory,
                                           const Dir& directory,
                                           const CommandLineArgs& commandLine,
                                           const Task& task) noexcept {
    Task result = task;
    result.append(PLUGIN_NAME);
    result.append({string("--").append(BASE_DIR_KEY), baseDirectory.native()});
    result.append({string("--").append(DIR_KEY), directory.native()});
    result.append("--zerocounters");
    result.append(commandLine);
    return result;
}

inline Lcov::Excludes
Lcov::getExcludes(const VariablesMap& variables) noexcept {
    auto excludes = variables.get<Excludes>(EXCLUDES_KEY);
    if(!excludes) {
        return Excludes();
    }

    for(auto& exclude : excludes.value()) {
        exclude.insert(0, R"(")");
        exclude.append(R"(")");
    }
    return excludes.value();
}

inline Task Lcov::generateExcludeTask(const VariablesMap& variables,
                                      const InfoFile& infoFile,
                                      const CommandLineArgs& commandLine,
                                      const Task& task) noexcept {
    Task result = task;
    auto excludes = getExcludes(variables);
    if(excludes.empty()) {
        return result;
    }
    result.append(PLUGIN_NAME);
    result.append({"--remove", infoFile.native()});
    result.append(excludes);
    result.append({"--output-file", infoFile.native()});
    result.append(commandLine);
    return result;
}

inline Task Lcov::generateCaptureTask(const BaseDir& baseDirectory,
                                      const Dir& directory,
                                      const InfoFile& infoFile,
                                      const CommandLineArgs& commandLine,
                                      const Task& task) noexcept {
    Task result = task;
    result.append(PLUGIN_NAME);
    result.append({string("--").append(BASE_DIR_KEY), baseDirectory.native()});
    result.append({string("--").append(DIR_KEY), directory.native()});
    result.append("--capture");
    result.append({"--output", infoFile.native()});
    result.append(commandLine);
    return result;
}
} // namespace plugins
} // namespace execHelper
