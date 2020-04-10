#ifndef FLEETING_OPTIONS_INTERFACE_INCLUDE
#define FLEETING_OPTIONS_INTERFACE_INCLUDE

#include <exception>
#include <string>

#include "commandLineOptions.h"
#include "path.h"

namespace execHelper::config {
/**
 * \brief   Class for collecting the values of variables that are expected to
 * change often
 */
class FleetingOptionsInterface {
  public:
    virtual ~FleetingOptionsInterface() = default;

    /**
     * Returns the status of the help setting
     *
     * \returns True    If the help setting was set
     *          False   Otherwise
     */
    virtual HelpOption_t getHelp() const noexcept = 0;

    /**
     * Returns whether the version option has been set
     *
     * \returns True    If the version option was set
     *          False   Otherwise
     */
    virtual VersionOption_t getVersion() const noexcept = 0;

    /**
     * Returns the status of the verbose setting
     *
     * \returns True    If verbose mode is set
     *          False   Otherwise
     */
    virtual VerboseOption_t getVerbosity() const noexcept = 0;

    /**
     * Returns whether dry run mode is set
     *
     * \returns True    If dry run mode is set
     *          False   Otherwise
     */
    virtual DryRunOption_t getDryRun() const noexcept = 0;

    /**
     * Returns the maximum number of jobs to use for a task
     *
     * \returns The number of jobs to use for a task
     */
    virtual Jobs_t getJobs() const noexcept = 0;

    /**
     * Return whether the plugins must be listed
     *
     * \returns True if the plugins must be listed
     *          False otherwise
     */
    virtual ListPluginsOption_t listPlugins() const noexcept = 0;

    /**
     * Return the additional search paths
     *
     * \returns A list of additional search paths
     */
    [[nodiscard]] virtual const Paths& appendedSearchPaths() const noexcept = 0;

    /**
     * Returns the commands that were set on the command line
     *
     * \returns A collection of commands given on the command line
     */
    virtual const CommandCollection& getCommands() const noexcept = 0;

  protected:
    FleetingOptionsInterface() = default;

    /*! @copydoc config::Argv::Argv(const config::Argv&)
     */
    FleetingOptionsInterface(const FleetingOptionsInterface& other) = default;

    /*! @copydoc config::Argv::Argv(config::Argv&&)
     */
    FleetingOptionsInterface(FleetingOptionsInterface&& other) = default;

    /*! @copydoc config::Argv::operator=(const config::Argv&)
     */
    FleetingOptionsInterface&
    operator=(const FleetingOptionsInterface& other) = default;

    /*! @copydoc config::Argv::operator=(config::Argv&&)
     */
    FleetingOptionsInterface&
    operator=(FleetingOptionsInterface&& other) = default;
};
} // namespace execHelper::config

#endif /* FLEETING_OPTIONS_INTERFACE_INCLUDE */
