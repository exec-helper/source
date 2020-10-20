#ifndef EXEC_HELPER_CONFIG_INCLUDE
#define EXEC_HELPER_CONFIG_INCLUDE

#include <stdexcept>

#include "log/logLevel.h"

#include "commandLineOptions.h"
#include "variablesMap.h"

#include "fleetingOptionsInterface.h"

namespace execHelper::config {
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

    /**
     * Copy constructor
     *
     * \param[in] other The object to copy from
     */
    FleetingOptions(const FleetingOptions& other) = default;

    /**
     * Move constructor
     *
     * \param[in] other The object to move from
     */
    FleetingOptions(FleetingOptions&& other) = default;

    virtual ~FleetingOptions() = default;

    FleetingOptions& operator=(const FleetingOptions& other) = delete;
    FleetingOptions& operator=(FleetingOptions&& other) = delete;

    /**
     * Equality operator
     *
     * \param[in] other The other object to compare with
     * \returns True    If the objects are considered equal
     *          False   Otherwise
     */
    auto operator==(const FleetingOptions& other) const -> bool;

    /**
     * Inequality operator
     *
     * \param[in] other The other object to compare with
     * \returns !operator==(other)
     */
    auto operator!=(const FleetingOptions& other) const -> bool;

    auto getHelp() const noexcept -> HelpOption_t override;
    auto getVersion() const noexcept -> VersionOption_t override;
    auto getVerbosity() const noexcept -> VerboseOption_t override;
    auto getDryRun() const noexcept -> DryRunOption_t override;
    auto getKeepGoing() const noexcept -> KeepGoingOption_t override;
    auto getJobs() const noexcept -> Jobs_t override;
    auto listPlugins() const noexcept -> ListPluginsOption_t override;
    [[nodiscard]] auto appendedSearchPaths() const noexcept
        -> const Paths& override;

    /**
     * Get the value of the log level option
     *
     * \returns The log level associated with the log level option
     */
    auto getLogLevel() const noexcept -> log::LogLevel override;

    auto getCommands() const noexcept -> const CommandCollection& override;

    auto getAutoComplete() const noexcept
        -> const std::optional<AutoCompleteOption_t>& override;

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
    const KeepGoingOption_t m_keepGoing;
    Jobs_t m_jobs;
    const LogLevelOption_t m_logLevel;
    const ListPluginsOption_t m_listPlugins;
    const Paths m_appendSearchPaths;
    CommandCollection m_commands;
    const std::optional<config::AutoCompleteOption_t> m_autocomplete;
};
} // namespace execHelper::config

#endif /* EXEC_HELPER_CONFIG_INCLUDE */
