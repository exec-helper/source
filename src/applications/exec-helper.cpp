#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "commander/commander.h"
#include "config/argv.h"
#include "config/commandLineOptions.h"
#include "config/config.h"
#include "config/configFileSearcher.h"
#include "config/envp.h"
#include "config/fleetingOptions.h"
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

#include "version.h"

using std::make_pair;
using std::make_shared;
using std::make_unique;
using std::move;
using std::optional;
using std::setw;
using std::string;
using std::stringstream;
using std::vector;

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
using execHelper::config::getAllParentDirectories;
using execHelper::config::getHomeDirectory;
using execHelper::config::HELP_OPTION_KEY;
using execHelper::config::HelpOption_t;
using execHelper::config::JOBS_KEY;
using execHelper::config::JobsOption_t;
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

namespace filesystem = std::filesystem;

namespace {
vector<string> logModules({"log", "yaml", "config", "core", "plugins",
                           "commander"});

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

inline bool verifyOptions(const FleetingOptions& options) noexcept {
    if(options.getJobs() == 0U) {
        user_feedback_error("Invalid value passed for the number of jobs (0): "
                            "it must be strictly positive.");
        return false;
    }
    return true;
}

inline optional<Path>
getSettingsFile(const Argv& argv, const EnvironmentCollection& env,
                const Option<SettingsFileOption_t>& settingsOption) noexcept {
    OptionDescriptions options;
    options.addOption(settingsOption);

    VariablesMap optionsMap = FleetingOptions::getDefault();
    if(!options.getOptionsMap(optionsMap, argv, true)) {
        user_feedback_error(
            "Could not properly parse the command line options");
        return std::nullopt;
    }
    FleetingOptions fleetingOptions(optionsMap);

    ConfigFileSearcher configFileSearcher(getSearchPaths(env));

#ifdef _WIN32
    SettingsFileOption_t settingsFileValue =
        optionsMap.get<SettingsFileOption_t>(SETTINGS_FILE_KEY,
                                             ".windows.exec-helper");
#else
    SettingsFileOption_t settingsFileValue =
        optionsMap.get<SettingsFileOption_t>(SETTINGS_FILE_KEY, ".exec-helper");
#endif
    auto settingsFile = configFileSearcher.find(settingsFileValue);
    if(settingsFile == std::nullopt) {
        if(!fleetingOptions.getHelp()) {
            user_feedback_error("Could not find a settings file");
        }
        return std::nullopt;
    }
    return settingsFile;
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
    options.addOption(settingsFileOption);
    options.addOption(
        Option<LogLevelOption_t>(LOG_LEVEL_KEY, {"d"}, "Set the log level"));
    options.addOption(commandOption);

    return options;
}

inline VariablesMap handleConfiguration(const Argv& argv,
                                        const EnvironmentCollection& /*env*/,
                                        OptionDescriptions& options) {
    options.setPositionalArgument(commandOption);
    VariablesMap optionsMap = FleetingOptions::getDefault();
    if(!options.getOptionsMap(optionsMap, argv, false)) {
        throw std::invalid_argument(
            "Could not properly parse the command line options");
    }
    return optionsMap;
}
} // namespace

int execHelperMain(int argc, char** argv, char** envp) {
    Argv args(argc, argv);
    EnvironmentCollection env = toEnvCollection(envp);

    auto optionDescriptions = getDefaultOptions();
    const auto settingsFile = getSettingsFile(args, env, settingsFileOption);

    PatternSettingsPair patternSettingsPair(Patterns(), SettingsNode("empty"));
    if(settingsFile) {
        try {
            patternSettingsPair =
                addPatternsFromSettingsFile(*settingsFile, optionDescriptions);
        } catch(const std::invalid_argument& e) {
            std::cerr << e.what() << std::endl;
        }
    }
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
    if(fleetingOptions.getHelp()) {
        printHelp(args[0], optionDescriptions, settings);
        return EXIT_SUCCESS;
    }

    if(fleetingOptions.getVersion()) {
        printVersion();
        return EXIT_SUCCESS;
    }

    if(!verifyOptions(fleetingOptions)) {
        return EXIT_FAILURE;
    }

    for(auto& pattern : patterns) {
        const auto longOption = pattern.getLongOption();
        if(longOption && optionsMap.contains(longOption.value())) {
            pattern.setValues(optionsMap.get<PatternValues>(
                longOption.value(), pattern.getValues()));
        }
    }

    if(!settingsFile) {
        printHelp(args[0], optionDescriptions, SettingsNode("empty"));
        return EXIT_FAILURE;
    }

    execHelper::log::LogInit logInit;
    auto level = fleetingOptions.getLogLevel();
    for(const auto& logModule : logModules) {
        logInit.setSeverity(logModule, level);
    }

    auto shell = make_shared<PosixShell>();
    execHelper::core::ImmediateExecutor::Callback callback =
        [](Shell::ShellReturnCode returnCode) {
            user_feedback_error("Error executing commands");
            { execHelper::log::LogInit logInit; }
            exit(returnCode);
        };
    std::unique_ptr<ExecutorInterface> executor;
    if(fleetingOptions.getDryRun()) {
        executor.reset(new ReportingExecutor());
    } else {
        executor = make_unique<ImmediateExecutor>(shell, callback);
    }

    execHelper::plugins::ExecuteCallback executeCallback =
        [executor = executor.get()](const Task& task) {
            try {
                executor->execute(task);
            } catch(const std::runtime_error& e) {
                user_feedback_error("Runtime error: " << e.what());
            }
        };
    execHelper::plugins::registerExecuteCallback(executeCallback);

    Commander commander;
    if(commander.run(fleetingOptions, settings, patterns,
                     settingsFile->parent_path(), move(env))) {

        return EXIT_SUCCESS;
    } else {
        user_feedback_error("Error executing commands");
        return EXIT_FAILURE;
    }
}

int main(int argc, char** argv, char** envp) {
    return execHelperMain(argc, argv, envp);
}
