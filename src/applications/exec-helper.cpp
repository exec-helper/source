#include <cstdlib>
#include <iomanip>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional/optional.hpp>

#include "commander/commander.h"
#include "config/argv.h"
#include "config/commandLineOptions.h"
#include "config/config.h"
#include "config/configFileSearcher.h"
#include "config/envp.h"
#include "config/fleetingOptions.h"
#include "config/optionDescriptions.h"
#include "config/pattern.h"
#include "config/settingsNode.h"
#include "config/variablesMap.h"
#include "core/executorInterface.h"
#include "core/immediateExecutor.h"
#include "core/posixShell.h"
#include "core/reportingExecutor.h"
#include "core/task.h"
#include "log/assertions.h"
#include "log/log.h"
#include "plugins/plugin.h"

using std::make_pair;
using std::make_shared;
using std::make_unique;
using std::move;
using std::setw;
using std::string;
using std::stringstream;
using std::vector;

using boost::filesystem::current_path;
using boost::optional;

using execHelper::commander::Commander;
using execHelper::config::ArgumentOption;
using execHelper::config::ArgumentOptions;
using execHelper::config::Argv;
using execHelper::config::COMMAND_KEY;
using execHelper::config::CommandCollection;
using execHelper::config::ConfigFileSearcher;
using execHelper::config::DRY_RUN_KEY;
using execHelper::config::DryRunOption_t;
using execHelper::config::EnvironmentCollection;
using execHelper::config::Envp;
using execHelper::config::FleetingOptions;
using execHelper::config::HELP_KEY;
using execHelper::config::HelpOption_t;
using execHelper::config::JOBS_KEY;
using execHelper::config::JobsOption_t;
using execHelper::config::LOG_LEVEL_KEY;
using execHelper::config::LogLevelOption_t;
using execHelper::config::Option;
using execHelper::config::OptionDescriptions;
using execHelper::config::Path;
using execHelper::config::Paths;
using execHelper::config::PatternValues;
using execHelper::config::Patterns;
using execHelper::config::SETTINGS_FILE_KEY;
using execHelper::config::SettingsFileOption_t;
using execHelper::config::SettingsFileOption_t;
using execHelper::config::SettingsNode;
using execHelper::config::VERBOSE_KEY;
using execHelper::config::VariablesMap;
using execHelper::config::VerboseOption_t;
using execHelper::config::parseSettingsFile;
using execHelper::core::ExecutorInterface;
using execHelper::core::ImmediateExecutor;
using execHelper::core::PosixShell;
using execHelper::core::ReportingExecutor;
using execHelper::core::Shell;
using execHelper::core::Task;
using execHelper::log::LogLevel;
using execHelper::log::setSeverity;

namespace {
vector<string> logModules({"log", "yaml", "config", "core", "plugins",
                           "commander"});

struct ConfigOptions {
    ConfigOptions(Path&& settingsFile, FleetingOptions&& fleetingOptions,
                  SettingsNode&& settings, Patterns&& patterns) noexcept
        : settingsFile(settingsFile),
          fleetingOptions(fleetingOptions),
          settings(settings),
          patterns(patterns) {
        ;
    }

    Path settingsFile;
    FleetingOptions fleetingOptions;
    SettingsNode settings;
    Patterns patterns;
};

inline EnvironmentCollection toEnvCollection(char** envp) {
    static const string DELIMITER("=");
    EnvironmentCollection result;
    size_t index = 0;
    char* envValue;
    while((envValue = envp[index]) != nullptr) {
        string newEnv(envValue);

        size_t pos = newEnv.find_first_of(DELIMITER);
        ensures(pos != newEnv.npos);

        string key = newEnv.substr(0, pos);
        string value = newEnv.substr(pos + DELIMITER.size(), newEnv.npos);
        result.emplace(make_pair(key, value));
        ++index;
    }
    return result;
}

inline Paths getSearchPaths(const EnvironmentCollection& env) {
    Paths searchPaths({current_path()});
    const string HOME_DIR_KEY("HOME");
    if(env.count(HOME_DIR_KEY) > 0) {
        searchPaths.push_back(env.at(HOME_DIR_KEY));
    }
    return searchPaths;
}

inline void printHelp(const OptionDescriptions& options,
                      const SettingsNode& settings) noexcept {
    static const string COMMANDS_KEY("commands");
    user_feedback(options.getOptionDescriptions());
    if(settings.contains(COMMANDS_KEY)) {
        user_feedback("Configured commands:");
        for(const auto& command : settings[COMMANDS_KEY].values()) {
            stringstream commandStream;
            commandStream << "  " << std::left << setw(20) << command;
            CommandCollection commmandDescription =
                settings[COMMANDS_KEY].get<CommandCollection>({command}, {});
            if(!commmandDescription.empty()) {
                // Add an extra whitespace in case the key is longer than the minimum width that was set
                commandStream << " " << commmandDescription.back();
            }
            user_feedback(commandStream.str());
        }
    }
}

inline bool verifyOptions(const FleetingOptions& options) noexcept {
    if(options.getJobs() == 0U) {
        user_feedback_error("Invalid value passed for the number of jobs (0): "
                            "it must be strictly positive.");
        return false;
    }
    return true;
}

inline boost::optional<Path>
getSettingsFile(const Argv& argv, const EnvironmentCollection& env,
                const Option<SettingsFileOption_t>& settingsOption) noexcept {
    OptionDescriptions options;
    options.addOption(settingsOption);

    VariablesMap optionsMap = FleetingOptions::getDefault();
    if(!options.getOptionsMap(optionsMap, argv, true)) {
        user_feedback_error(
            "Could not properly parse the command line options");
        return boost::none;
    }
    FleetingOptions fleetingOptions(optionsMap);

    ConfigFileSearcher configFileSearcher(getSearchPaths(env));
    SettingsFileOption_t settingsFileValue =
        optionsMap.get<SettingsFileOption_t>(SETTINGS_FILE_KEY, ".exec-helper");
    boost::optional<Path> settingsFile =
        configFileSearcher.find(settingsFileValue);
    if(settingsFile == boost::none) {
        if(!fleetingOptions.getHelp()) {
            user_feedback("Could not find a settings file");
        }
        printHelp(options, SettingsNode("empty"));
        return boost::none;
    }
    return settingsFile;
}

inline optional<ConfigOptions>
handleConfiguration(const Argv& argv,
                    const EnvironmentCollection& env) noexcept {
    OptionDescriptions options;
    options.addOption(
        Option<HelpOption_t>(HELP_KEY, {"h"}, "Produce help message"));
    options.addOption(
        Option<VerboseOption_t>(VERBOSE_KEY, {"v"}, "Set verbosity"));
    options.addOption(Option<JobsOption_t>(
        JOBS_KEY, {"j"}, "Set number of jobs to use. Default: auto"));
    options.addOption(
        Option<DryRunOption_t>(DRY_RUN_KEY, {"n"}, "Dry run exec-helper"));
    auto settingsFileOption = Option<SettingsFileOption_t>(
        SETTINGS_FILE_KEY, {"s"}, "Set the settings file");
    options.addOption(settingsFileOption);
    options.addOption(
        Option<LogLevelOption_t>(LOG_LEVEL_KEY, {"d"}, "Set the log level"));
    auto commandOption =
        Option<CommandCollection>(COMMAND_KEY, {"z"}, "Commands to execute");
    options.addOption(commandOption);

    auto settingsFile = getSettingsFile(argv, env, settingsFileOption);
    if(!settingsFile) {
        return boost::none;
    }

    auto patternSettingsPair = parseSettingsFile(settingsFile.get());
    if(!patternSettingsPair) {
        user_feedback("Could not parse settings file '" << settingsFile.get()
                                                        << "'");
        return boost::none;
    }

    auto patterns = patternSettingsPair.get().first;
    auto settings = patternSettingsPair.get().second;

    for(const auto& pattern : patterns) {
        ArgumentOptions additionalArguments;
        const auto shortOption = pattern.getShortOption();
        if(shortOption) {
            additionalArguments.emplace_back(
                ArgumentOption(1, shortOption.get()));
        }
        const auto longOption = pattern.getLongOption();
        if(longOption) {
            options.addOption(Option<PatternValues>(
                longOption.get(), additionalArguments,
                "Values for pattern '" + longOption.get() + "'"));
        }
    }

    options.setPositionalArgument(commandOption);
    VariablesMap optionsMap = FleetingOptions::getDefault();
    if(!options.getOptionsMap(optionsMap, argv, true)) {
        user_feedback_error(
            "Could not properly parse the command line options");
        return boost::none;
    }

    FleetingOptions fleetingOptions(optionsMap);
    if(fleetingOptions.getHelp()) {
        printHelp(options, settings);
        return boost::none;
    }

    if(!verifyOptions(fleetingOptions)) {
        return boost::none;
    }

    for(auto& pattern : patterns) {
        const auto longOption = pattern.getLongOption();
        if(longOption && optionsMap.contains(longOption.get())) {
            pattern.setValues(optionsMap.get<PatternValues>(
                longOption.get(), pattern.getValues()));
        }
    }

    return ConfigOptions(move(settingsFile.get()), move(fleetingOptions),
                         move(settings), move(patterns));
}
} // namespace

int execHelperMain(int argc, char** argv, char** envp) {
    Argv args(argc, argv);
    EnvironmentCollection env = toEnvCollection(envp);
    auto config = handleConfiguration(args, env);
    if(!config) {
        return EXIT_FAILURE;
    }
    const Path& settingsFile = config.get().settingsFile;
    const FleetingOptions& fleetingOptions = config.get().fleetingOptions;
    Patterns patterns = config.get().patterns;
    SettingsNode settings = config.get().settings;

    execHelper::log::init();
    auto level = fleetingOptions.getLogLevel();
    for(const auto& logModule : logModules) {
        setSeverity(logModule, level);
    }

    auto shell = make_shared<PosixShell>();
    execHelper::core::ImmediateExecutor::Callback callback =
        [](Shell::ShellReturnCode returnCode) {
            user_feedback_error("Error executing commands");
            exit(returnCode);
        };
    std::unique_ptr<ExecutorInterface> executor;
    if(fleetingOptions.getDryRun()) {
        executor.reset(new ReportingExecutor());
    } else {
        executor = make_unique<ImmediateExecutor>(shell, callback);
    }

    execHelper::plugins::ExecuteCallback executeCallback = [executor =
                                                                executor.get()](
        const Task& task) {
        executor->execute(task);
    };
    execHelper::plugins::registerExecuteCallback(executeCallback);

    Commander commander;
    if(commander.run(fleetingOptions, move(settings), move(patterns),
                     settingsFile.parent_path(), move(env))) {
        return EXIT_SUCCESS;
    } else {
        user_feedback_error("Error executing commands");
        return EXIT_FAILURE;
    }
}

int main(int argc, char** argv, char** envp) {
    return execHelperMain(argc, argv, envp);
}
