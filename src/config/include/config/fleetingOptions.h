#ifndef EXEC_HELPER_CONFIG_INCLUDE
#define EXEC_HELPER_CONFIG_INCLUDE

#include <stdexcept>

#include "log/logLevel.h"

#include "commandLineOptions.h"
#include "variablesMap.h"

#include "fleetingOptionsInterface.h"

namespace execHelper {
namespace config {
/**
 * \brief Options that are as fleeting as the wind
 */
class FleetingOptions : public FleetingOptionsInterface {
  public:
    /**
     * Create fleeting options
     *
     * \param[in] optionsMap   The variables map to get the fleeting options from
     */
    explicit FleetingOptions(const VariablesMap& optionsMap) noexcept;

    /*! @copydoc config::Argv::Argv(const config::Argv& other)
     */
    FleetingOptions(const FleetingOptions& other) = default;

    /*! @copydoc config::Argv::Argv(config::Argv&& other)
     */
    FleetingOptions(FleetingOptions&& other) = default;
    virtual ~FleetingOptions() = default;

    /*! @copydoc config::Argv::operator=(const config::Argv& other)
     */
    FleetingOptions& operator=(const FleetingOptions& other) = delete;

    /*! @copydoc config::Argv::operator=(config::Argv&& other)
     */
    FleetingOptions& operator=(FleetingOptions&& other) = delete;

    /*! @copydoc config::Argv::operator==(const config::Argv& other) const
     */
    auto operator==(const FleetingOptions& other) const -> bool;

    /*! @copydoc config::Argv::operator!=(const config::Argv& other) const
     */
    auto operator!=(const FleetingOptions& other) const -> bool;

    auto getHelp() const noexcept -> HelpOption_t override;
    auto getVersion() const noexcept -> VersionOption_t override;
    auto getVerbosity() const noexcept -> VerboseOption_t override;
    auto getDryRun() const noexcept -> DryRunOption_t override;
    auto getJobs() const noexcept -> Jobs_t override;
    auto listPlugins() const noexcept -> ListPluginsOption_t override;
    [[nodiscard]] auto appendedSearchPaths() const noexcept
        -> const Paths& override;

    /**
     * Get the value of the log level option
     *
     * \returns The log level associated with the log level option
     */
    auto getLogLevel() const noexcept -> log::LogLevel;

    auto getCommands() const noexcept -> const CommandCollection& override;

    /**
     * Returns the default variables for the fleeting options
     *
     * \returns The default variables
     */
    static auto getDefault() noexcept -> VariablesMap;

  private:
    const HelpOption_t m_help;
    const VersionOption_t m_version;
    const VerboseOption_t m_verbose;
    const DryRunOption_t m_dryRun;
    Jobs_t m_jobs;
    const LogLevelOption_t m_logLevel;
    const ListPluginsOption_t m_listPlugins;
    const Paths m_appendSearchPaths;
    CommandCollection m_commands;
};
} // namespace config
} // namespace execHelper

#endif /* EXEC_HELPER_CONFIG_INCLUDE */
