#include <algorithm>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iomanip>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "commander/commander.h"
#include "config/commandLineOptions.h"
#include "config/config.h"
#include "config/configFileSearcher.h"
#include "config/fleetingOptions.h"
#include "config/fleetingOptionsInterface.h"
#include "config/optionDescriptions.h"
#include "config/pathManipulation.h"
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
#include "plugins/pluginUtils.h"

#include "logger.h"
#include "version.h"

using std::exception;
using std::make_pair;
using std::make_shared;
using std::make_unique;
using std::move;
using std::optional;
using std::runtime_error;
using std::setw;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::transform;
using std::vector;

using execHelper::commander::Commander;
using execHelper::config::APPEND_SEARCH_PATH_KEY;
using execHelper::config::AppendSearchPathOption_t;
using execHelper::config::ArgumentOption;
using execHelper::config::ArgumentOptions;
using execHelper::config::Args;
using execHelper::config::AUTO_COMPLETE_KEY;
using execHelper::config::AutoCompleteOption_t;
using execHelper::config::COMMAND_KEY;
using execHelper::config::CommandCollection;
using execHelper::config::ConfigFileSearcher;
using execHelper::config::DRY_RUN_KEY;
using execHelper::config::DryRunOption_t;
using execHelper::config::EnvironmentCollection;
using execHelper::config::FleetingOptions;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::getAllParentDirectories;
using execHelper::config::getHomeDirectory;
using execHelper::config::HELP_OPTION_KEY;
using execHelper::config::HelpOption_t;
using execHelper::config::JOBS_KEY;
using execHelper::config::JobsOption_t;
using execHelper::config::KEEP_GOING_KEY;
using execHelper::config::KeepGoingOption_t;
using execHelper::config::LIST_PLUGINS_KEY;
using execHelper::config::ListPluginsOption_t;
using execHelper::config::LOG_LEVEL_KEY;
using execHelper::config::LogLevelOption_t;
using execHelper::config::Option;
using execHelper::config::OptionDescriptions;
using execHelper::config::parseSettingsFile;
using execHelper::config::Path;
using execHelper::config::Paths;
using execHelper::config::Patterns;
using execHelper::config::PatternSettingsPair;
using execHelper::config::PatternValues;
using execHelper::config::SETTINGS_FILE_KEY;
using execHelper::config::SettingsFileOption_t;
using execHelper::config::SettingsNode;
using execHelper::config::SettingsValues;
using execHelper::config::VariablesMap;
using execHelper::config::VERBOSE_KEY;
using execHelper::config::VerboseOption_t;
using execHelper::config::VERSION_KEY;
using execHelper::config::VersionOption_t;
using execHelper::core::ExecutorInterface;
using execHelper::core::ImmediateExecutor;
using execHelper::core::PosixShell;
using execHelper::core::ReportingExecutor;
using execHelper::core::Shell;
using execHelper::core::Task;
using execHelper::log::LogLevel;
using execHelper::plugins::discoverPlugins;
using execHelper::plugins::discoverPluginSummaries;
using execHelper::plugins::Plugins;
using execHelper::plugins::PluginSummaries;
using execHelper::plugins::makePatternPermutator;
using execHelper::plugins::replacePatternCombinations;

namespace filesystem = std::filesystem;

namespace {
vector<string> logModules({"log", "config", "core", "plugins",
                           "commander", "application"});

const auto settingsFileOption = Option<SettingsFileOption_t>(
    SETTINGS_FILE_KEY, {"s"}, "Set the settings file");
const auto commandOption =
    Option<CommandCollection>(COMMAND_KEY, {"z"}, "Commands to execute");

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

inline Paths getSearchPaths(const EnvironmentCollection& env) noexcept {
    Paths searchPaths = getAllParentDirectories(filesystem::current_path());
    auto homeDir = getHomeDirectory(env);
    if(homeDir) {
        searchPaths.emplace_back(homeDir.value());
    }
    return searchPaths;
}

inline execHelper::config::Paths
addAdditionalSearchPaths(execHelper::config::Paths& base,
                         const execHelper::config::Paths& toAdd,
                         const Path& basePath) noexcept {
    transform(toAdd.rbegin(), toAdd.rend(), back_inserter(base),
              [&basePath](auto path) {
                  if(path.is_relative()) {
                      path = basePath / path;
                  }

                  LOG(trace) << "Adding search path " << path;
                  return path;
              });
    return base;
}

inline execHelper::config::Paths
addAdditionalSearchPaths(execHelper::config::Paths& base,
                         const vector<string>& toAdd,
                         const Path& basePath) noexcept {
    Paths paths;
    transform(toAdd.begin(), toAdd.end(), back_inserter(paths),
              [](const auto& path) { return Path{path}; });
    return addAdditionalSearchPaths(base, paths, basePath);
}

inline execHelper::config::Paths
getAdditionalSearchPaths(const FleetingOptionsInterface& fleetingOptions,
                         const SettingsNode& settings,
                         const Path& basePath) noexcept {
    constexpr std::string_view configKey{"additional-search-paths"};
    Paths pluginSearchPath{PLUGINS_INSTALL_PATH};
    if(settings.contains(string(configKey))) {
        pluginSearchPath = addAdditionalSearchPaths(
            pluginSearchPath,
            settings.get<vector<string>>(string(configKey), {}), basePath);
    }
    return addAdditionalSearchPaths(
        pluginSearchPath, fleetingOptions.appendedSearchPaths(), basePath);
}

inline auto printAutoComplete(const std::string& /*word*/,
                              const OptionDescriptions& options,
                              const vector<string>& commands) noexcept -> void {
    for(const auto& key : options.getOptionKeys()) {
        user_feedback(key);
    }
    for(const auto& command : commands) {
        user_feedback(command);
    }
}

inline void printHelp(const std::string& binaryName,
                      const OptionDescriptions& options,
                      const SettingsNode& settings) noexcept {
    user_feedback("Usage: " + binaryName + " [Optional arguments] COMMANDS...");
    user_feedback("");

    user_feedback("Optional arguments:");
    user_feedback(options.getOptionDescriptions());

    static const string COMMANDS_KEY("commands");
    if(settings.contains(COMMANDS_KEY)) {
        user_feedback("Configured commands:");
        for(const auto& command :
            settings.get<SettingsValues>(COMMANDS_KEY, SettingsValues())) {
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

inline void printVersion() noexcept {
    user_feedback(BINARY_NAME << " " << VERSION);
    user_feedback(COPYRIGHT);
}

inline void printPlugins(const PluginSummaries& plugins) noexcept {
    user_feedback("Registered plugins:");
    for(const auto& plugin : plugins) {
        user_feedback(std::left << std::setfill('.') << std::setw(25)
                                << plugin.first << " " << plugin.second);
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

inline auto getSettingsFile(const std::string& settingsFilename,
                            const EnvironmentCollection& env) -> Path {
    ConfigFileSearcher configFileSearcher(getSearchPaths(env));

    auto settingsFile = configFileSearcher.find(settingsFilename);
    if(settingsFile == std::nullopt) {
        throw std::runtime_error("Could not find a settings file");
    }
    return *settingsFile;
}

PatternSettingsPair addPatternsFromSettingsFile(const Path& settingsFile,
                                                OptionDescriptions& options) {
    auto patternSettingsPair = parseSettingsFile(settingsFile);
    if(!patternSettingsPair) {
        throw std::invalid_argument("Could not parse settings file '" +
                                    settingsFile.string() + "'");
    }

    auto patterns = patternSettingsPair.value().first;

    for(const auto& pattern : patterns) {
        ArgumentOptions additionalArguments;
        const auto shortOption = pattern.getShortOption();
        if(shortOption) {
            additionalArguments.emplace_back(
                ArgumentOption(1, shortOption.value()));
        }
        const auto longOption = pattern.getLongOption();
        if(longOption) {
            options.addOption(Option<PatternValues>(
                longOption.value(), additionalArguments,
                "Values for pattern '" + longOption.value() + "'"));
        }
    }
    return *patternSettingsPair;
}

inline OptionDescriptions getDefaultOptions() noexcept {
    OptionDescriptions options;
    options.addOption(
        Option<HelpOption_t>(HELP_OPTION_KEY, {"h"}, "Produce help message"));
    options.addOption(Option<HelpOption_t>(VERSION_KEY, {},
                                           "Print the version of this binary"));
    options.addOption(
        Option<VerboseOption_t>(VERBOSE_KEY, {"v"}, "Set verbosity"));
    options.addOption(Option<JobsOption_t>(
        JOBS_KEY, {"j"}, "Set number of jobs to use. Default: auto"));
    options.addOption(
        Option<DryRunOption_t>(DRY_RUN_KEY, {"n"}, "Dry run exec-helper"));
    options.addOption(
        Option<KeepGoingOption_t>(KEEP_GOING_KEY, {"k"}, "Keep going, even when commands fail"));
    options.addOption(
        Option<ListPluginsOption_t>(LIST_PLUGINS_KEY, {}, "List all plugins"));
    options.addOption(Option<AppendSearchPathOption_t>(
        APPEND_SEARCH_PATH_KEY, {},
        "Append to plugin search path. Plugins discovered earlier in the list "
        "overwrite plugins with the same name in later ones."));
    options.addOption(settingsFileOption);
    options.addOption(
        Option<LogLevelOption_t>(LOG_LEVEL_KEY, {"d"}, "Set the log level"));
    options.addOption(Option<AutoCompleteOption_t>(
        string(AUTO_COMPLETE_KEY), {},
        "List autocomplete options for the given word"));
    options.addOption(commandOption);

    return options;
}

inline VariablesMap handleConfiguration(const Args& args,
                                        const EnvironmentCollection& /*env*/,
                                        OptionDescriptions& options) {
    options.setPositionalArgument(commandOption);
    VariablesMap optionsMap = FleetingOptions::getDefault();
    if(!options.getOptionsMap(optionsMap, args, false)) {
        throw std::invalid_argument(
            "Could not properly parse the command line options");
    }
    return optionsMap;
}
} // namespace

int execHelperMain(int argc, char** argv, char** envp) {
    const Args args(argv, argc);
    const EnvironmentCollection env = toEnvCollection(envp);

    auto firstPassOptions = getDefaultOptions();

    VariablesMap firstPassOptionsMap = FleetingOptions::getDefault();
    if(!firstPassOptions.getOptionsMap(firstPassOptionsMap, args, true)) {
        user_feedback_error(
            "Could not properly parse the command line options");
        printHelp(args[0], firstPassOptions, SettingsNode("Options"));
        return EXIT_FAILURE;
    }
    FleetingOptions firstPassFleetingOptions(firstPassOptionsMap);

    Path settingsFile;
    try {
#ifdef _WIN32
        SettingsFileOption_t settingsFileValue =
            firstPassOptionsMap.get<SettingsFileOption_t>(
                SETTINGS_FILE_KEY, ".windows.exec-helper");
#else
        SettingsFileOption_t settingsFileValue =
            firstPassOptionsMap.get<SettingsFileOption_t>(SETTINGS_FILE_KEY,
                                                          ".exec-helper");
#endif

        settingsFile = getSettingsFile(settingsFileValue, env);
    } catch(const runtime_error&) {
        if(firstPassFleetingOptions.getHelp()) {
            printHelp(args[0], firstPassOptions, SettingsNode("Options"));
            return EXIT_SUCCESS;
        }

        if(firstPassFleetingOptions.getVersion()) {
            printVersion();
            return EXIT_SUCCESS;
        }

        if(firstPassFleetingOptions.listPlugins()) {
            auto pluginSearchPath = getAdditionalSearchPaths(
                firstPassFleetingOptions, SettingsNode("error"),
                filesystem::current_path());
            auto summaries = discoverPluginSummaries(pluginSearchPath);
            printPlugins(summaries);
            return EXIT_SUCCESS;
        }

        if(firstPassFleetingOptions.getAutoComplete()) {
            printAutoComplete(*(firstPassFleetingOptions.getAutoComplete()),
                              firstPassOptions, {});
            return EXIT_SUCCESS;
        }

        user_feedback_error("Could not find an exec-helper settings file");
        printHelp(args[0], firstPassOptions, SettingsNode("Options"));
        return EXIT_FAILURE;
    }

    auto optionDescriptions = getDefaultOptions();
    auto patternSettingsPair =
        addPatternsFromSettingsFile(settingsFile, optionDescriptions);

    auto patterns = patternSettingsPair.first;
    auto settings = patternSettingsPair.second;

    VariablesMap optionsMap("options");

    try {
        optionsMap = handleConfiguration(args, env, optionDescriptions);
    } catch(const std::invalid_argument&) {
        user_feedback_error(
            "Could not properly parse the command line options");
        printHelp(args[0], optionDescriptions, settings);
        return EXIT_FAILURE;
    }

    const FleetingOptions fleetingOptions(optionsMap);
    execHelper::log::LogInit logInit;
    auto level = fleetingOptions.getLogLevel();
    for(const auto& logModule : logModules) {
        logInit.setSeverity(logModule, level);
    }

    // All files are taken relative to this basePath
    const auto basePath = settingsFile.parent_path();
    LOG(debug) << "Base path is " << basePath;

    if(fleetingOptions.getHelp()) {
        printHelp(args[0], optionDescriptions, settings);
        return EXIT_SUCCESS;
    }

    if(fleetingOptions.getVersion()) {
        printVersion();
        return EXIT_SUCCESS;
    }

    auto pluginSearchPath =
        getAdditionalSearchPaths(fleetingOptions, settings, basePath);

    if(fleetingOptions.listPlugins()) {
        auto summaries = discoverPluginSummaries(pluginSearchPath);
        printPlugins(summaries);
        return EXIT_SUCCESS;
    }

    auto plugins = discoverPlugins(pluginSearchPath);
    if(firstPassFleetingOptions.getAutoComplete()) {
        vector<string> commands = settings.get<vector<string>>("commands", {});
        printAutoComplete(*(fleetingOptions.getAutoComplete()),
                          optionDescriptions, move(commands));
        return EXIT_SUCCESS;
    }

    if(!verifyOptions(fleetingOptions)) {
        return EXIT_FAILURE;
    }

    for(auto& pattern : patterns) {
        const auto longOption = pattern.getLongOption();
        if(longOption && optionsMap.contains(longOption.value())) {
            if(!pattern.setValues(optionsMap.get<PatternValues>(
                   longOption.value(), pattern.getValues()))) {
                LOG(error) << "Failed to add long options for '"
                           << longOption.value() << "'";
            }
        }
    }

    PosixShell shell;
    std::unique_ptr<ExecutorInterface> executor;
    auto lastReturnCode = EXIT_SUCCESS;
    if(fleetingOptions.getDryRun()) {
        executor.reset(new ReportingExecutor());
    } else if(fleetingOptions.getKeepGoing()) {
        executor = make_unique<ImmediateExecutor>(shell,  [&lastReturnCode](Shell::ShellReturnCode returnCode) {
            lastReturnCode = returnCode;
            user_feedback_error("Error executing command!");
        });
    } else {
        executor = make_unique<ImmediateExecutor>(shell,  [](Shell::ShellReturnCode returnCode) {
            user_feedback_error("Error executing command!");
            { execHelper::log::LogInit logInit; }       // Make sure the loggers are destroyed before exiting
            exit(returnCode);
        });
    }

    Commander commander;
    try {
        auto tasks = commander.run(fleetingOptions, settings, patterns,
                     settingsFile.parent_path(), move(env), move(plugins), settingsFile.parent_path());

        for(const auto& task : tasks) {
            for(const auto& combination : makePatternPermutator(task.getPatterns())) {
                Task newTask = replacePatternCombinations(task, combination);
                executor->execute(newTask);
            }
        }
    } catch(const exception& e) {
        user_feedback_error("Error executing commands: " << e.what());
        return EXIT_FAILURE;
    }
    return lastReturnCode;
}

int main(int argc, char** argv, char** envp) {
    return execHelperMain(argc, argv, envp);
}
