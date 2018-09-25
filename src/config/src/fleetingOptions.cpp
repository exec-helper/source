#include "fleetingOptions.h"

#include <thread>

#include <boost/lexical_cast.hpp>

#include "logger.h"

using std::thread;

using boost::lexical_cast;

using execHelper::log::LogLevel;

namespace execHelper {
namespace config {
FleetingOptions::FleetingOptions(const VariablesMap& optionsMap) noexcept
    : m_help(optionsMap.get<HelpOption_t>(HELP_KEY).value_or(false)),
      m_version(optionsMap.get<VersionOption_t>(VERSION_KEY).value_or(false)),
      m_verbose(optionsMap.get<VerboseOption_t>(VERBOSE_KEY).value_or(false)),
      m_dryRun(optionsMap.get<DryRunOption_t>(DRY_RUN_KEY).value_or(false)),
      m_jobs(1U),
      m_logLevel(
          optionsMap.get<LogLevelOption_t>(LOG_LEVEL_KEY).value_or("warning")),
      m_commands(optionsMap.get<CommandCollection>(COMMAND_KEY)
                     .value_or(CommandCollection())) {
    auto jobs = optionsMap.get<JobsOption_t>(JOBS_KEY).value_or("auto");
    if(jobs == "auto") {
        m_jobs = thread::hardware_concurrency();
    } else {
        try {
            m_jobs = lexical_cast<Jobs_t>(jobs);
        } catch(const boost::bad_lexical_cast& e) {
            LOG(warning)
                << "Bad lexical cast for the number of jobs. Using default.";
        }
    }
}

bool FleetingOptions::operator==(const FleetingOptions& other) const {
    return m_help == other.m_help && m_verbose == other.m_verbose &&
           m_dryRun == other.m_dryRun && m_jobs == other.m_jobs &&
           m_logLevel == other.m_logLevel && m_commands == other.m_commands;
}

bool FleetingOptions::operator!=(const FleetingOptions& other) const {
    return !(*this == other);
}

HelpOption_t FleetingOptions::getHelp() const noexcept { return m_help; }

VersionOption_t FleetingOptions::getVersion() const noexcept {
    return m_version;
}

VerboseOption_t FleetingOptions::getVerbosity() const noexcept {
    return m_verbose;
}

DryRunOption_t FleetingOptions::getDryRun() const noexcept { return m_dryRun; }

Jobs_t FleetingOptions::getJobs() const noexcept { return m_jobs; }

const CommandCollection& FleetingOptions::getCommands() const noexcept {
    return m_commands;
}

LogLevel FleetingOptions::getLogLevel() const noexcept {
    try {
        return log::toLogLevel(m_logLevel);
    } catch(const log::InvalidLogLevel& e) {
        LOG(warning) << "Invalid log level given. Using default.";
        return log::none;
    }
}

VariablesMap FleetingOptions::getDefault() noexcept {
    VariablesMap defaults("exec-helper");
    defaults.add(HELP_KEY, "no");
    defaults.add(VERSION_KEY, "no");
    defaults.add(VERBOSE_KEY, "no");
    defaults.add(DRY_RUN_KEY, "no");
    defaults.add(JOBS_KEY, "auto");
    defaults.add(SETTINGS_FILE_KEY);
    defaults.add(LOG_LEVEL_KEY, "none");
    defaults.add(COMMAND_KEY, CommandCollection());
    return defaults;
}
} // namespace config
} // namespace execHelper
