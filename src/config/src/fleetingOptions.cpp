#include "fleetingOptions.h"

#include <algorithm>
#include <thread>

#include <boost/lexical_cast.hpp>

#include "commandLineOptions.h"
#include "log/logLevel.h"
#include "logger.h"

using std::back_inserter;
using std::thread;
using std::transform;

using boost::lexical_cast;

using execHelper::log::LogLevel;

namespace  {
inline execHelper::config::Paths toPaths(const std::vector<std::string>& toConvert) noexcept {
    execHelper::config::Paths paths;
    transform(toConvert.begin(), toConvert.end(), back_inserter(paths), [](const auto& to) {
        return execHelper::config::Path{to};
    });
    return paths;
}
} // namespace 

namespace execHelper::config {
FleetingOptions::FleetingOptions(const VariablesMap& optionsMap) noexcept
    : m_help(optionsMap.get<HelpOption_t>(HELP_OPTION_KEY).value_or(false)),
      m_version(optionsMap.get<VersionOption_t>(VERSION_KEY).value_or(false)),
      m_verbose(optionsMap.get<VerboseOption_t>(VERBOSE_KEY).value_or(false)),
      m_dryRun(optionsMap.get<DryRunOption_t>(DRY_RUN_KEY).value_or(false)),
      m_jobs(1U),
      m_logLevel(
          optionsMap.get<LogLevelOption_t>(LOG_LEVEL_KEY).value_or("warning")),
      m_listPlugins(optionsMap.get<ListPluginsOption_t>(LIST_PLUGINS_KEY)
                        .value_or(false)),
      m_appendSearchPaths(toPaths(optionsMap.get<AppendSearchPathOption_t>(APPEND_SEARCH_PATH_KEY).value_or(AppendSearchPathOption_t()))),
      m_commands(optionsMap.get<CommandCollection>(COMMAND_KEY)
                     .value_or(CommandCollection())) {
    auto jobs = optionsMap.get<JobsOption_t>(JOBS_KEY).value_or("auto");
    if(jobs == "auto") {
        m_jobs = thread::hardware_concurrency();
    } else {
        try {
            m_jobs = lexical_cast<Jobs_t>(jobs);
        } catch(const boost::bad_lexical_cast&) {
            LOG(warning)
                << "Bad lexical cast for the number of jobs. Using default.";
        }
    }
}

auto FleetingOptions::operator==(const FleetingOptions& other) const -> bool {
    return m_help == other.m_help && m_verbose == other.m_verbose &&
           m_dryRun == other.m_dryRun && m_jobs == other.m_jobs &&
           m_logLevel == other.m_logLevel && m_commands == other.m_commands;
}

auto FleetingOptions::operator!=(const FleetingOptions& other) const -> bool {
    return !(*this == other);
}

auto FleetingOptions::getHelp() const noexcept -> HelpOption_t {
    return m_help;
}

auto FleetingOptions::getVersion() const noexcept -> VersionOption_t {
    return m_version;
}

auto FleetingOptions::getVerbosity() const noexcept -> VerboseOption_t {
    return m_verbose;
}

auto FleetingOptions::getDryRun() const noexcept -> DryRunOption_t {
    return m_dryRun;
}

auto FleetingOptions::getJobs() const noexcept -> Jobs_t { return m_jobs; }

auto FleetingOptions::getCommands() const noexcept -> const CommandCollection& {
    return m_commands;
}

auto FleetingOptions::getLogLevel() const noexcept -> LogLevel {
    try {
        return log::toLogLevel(m_logLevel);
    } catch(const log::InvalidLogLevel&) {
        LOG(warning) << "Invalid log level given. Using default.";
        return log::none;
    }
}

auto FleetingOptions::listPlugins() const noexcept -> ListPluginsOption_t {
    return m_listPlugins;
}

auto FleetingOptions::appendedSearchPaths() const noexcept -> const Paths& {
    return m_appendSearchPaths;
}

auto FleetingOptions::getDefault() noexcept -> VariablesMap {
    VariablesMap defaults("exec-helper");
    if(!defaults.add(HELP_OPTION_KEY, "no")) {
        LOG(error) << "Failed to add help default option value";
    }
    if(!defaults.add(VERSION_KEY, "no")) {
        LOG(error) << "Failed to add version default option value";
    }
    if(!defaults.add(VERBOSE_KEY, "no")) {
        LOG(error) << "Failed to add verbose default option value";
    }
    if(!defaults.add(DRY_RUN_KEY, "no")) {
        LOG(error) << "Failed to add dry run default option value";
    }
    if(!defaults.add(JOBS_KEY, "auto")) {
        LOG(error) << "Failed to add jobs default option value";
    }
    if(!defaults.add(SETTINGS_FILE_KEY)) {
        LOG(error) << "Failed to add settings file default option value";
    }
    if(!defaults.add(LOG_LEVEL_KEY, "none")) {
        LOG(error) << "Failed to add log level default option value";
    }
    if(!defaults.add(LIST_PLUGINS_KEY, "no")) {
        LOG(error) << "Failed to add 'list plugins' default option value";
    }
    if(!defaults.add(APPEND_SEARCH_PATH_KEY, AppendSearchPathOption_t())) {
        LOG(error) << "Failed to add 'append-search-path' default option value";
    }
    if(!defaults.add(COMMAND_KEY, CommandCollection())) {
        LOG(error) << "Failed to add commands default option value";
    }
    return defaults;
}
} // namespace execHelper::config
