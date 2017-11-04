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
                FleetingOptions& operator=(const FleetingOptions& other) = default;

                /*! @copydoc config::Argv::operator=(config::Argv&& other)
                 */
                FleetingOptions& operator=(FleetingOptions&& other) = default;

                /*! @copydoc config::Argv::operator==(const config::Argv& other) const
                 */
                bool operator==(const FleetingOptions& other);

                /*! @copydoc config::Argv::operator!=(const config::Argv& other) const
                 */
                bool operator!=(const FleetingOptions& other);

                /**
                 * Get the value of the help option
                 *
                 * \returns The value for the help option
                 */
                HelpOption_t getHelp() const noexcept override;

                /**
                 * Get the value of the verbosity option
                 *
                 * \returns The value for the verbosity option
                 */
                VerboseOption_t getVerbosity() const noexcept override;

                /**
                 * Get the value of the dry-run option
                 *
                 * \returns The value for the dry-run option
                 */
                DryRunOption_t getDryRun() const noexcept override;

                /**
                 * Get the number of jobs to use
                 *
                 * \returns The value for the number of jobs
                 */
                Jobs_t getJobs() const noexcept override;

                /**
                 * Get the value of the log level option
                 *
                 * \returns The log level associated with the log level option
                 */
                log::LogLevel getLogLevel() const noexcept;

                /**
                 * Get the value of the commands option
                 *
                 * \return The values for the command option
                 */
                const CommandCollection& getCommands() const noexcept override;

                /**
                 * Returns the default variables for the fleeting options
                 *
                 * \returns The default variables
                 */
                static VariablesMap getDefault() noexcept;

            private:
                const HelpOption_t m_help;
                const VerboseOption_t m_verbose;
                const DryRunOption_t m_dryRun;
                Jobs_t m_jobs;
                const LogLevelOption_t m_logLevel;
                CommandCollection m_commands;
        };
    } // config
} // execHelper

#endif  /* EXEC_HELPER_CONFIG_INCLUDE */
