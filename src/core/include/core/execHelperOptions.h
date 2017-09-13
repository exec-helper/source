#ifndef __EXEC_HELPER_OPTIONS_H__
#define __EXEC_HELPER_OPTIONS_H__

#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "options.h"

#include "config/settingsNode.h"

#include "optionDescriptions.h"
#include "patternsHandler.h"

namespace execHelper {
    namespace core {
        class OptionDescriptions;
    } // namespace core
} // namespace execHelper

namespace execHelper {
    namespace core {
        /**
         * \brief Implements the options for exec-helper
         */
        class ExecHelperOptions : public Options {
            public:
                ExecHelperOptions() noexcept;

		/*! @copydoc Argv::Argv(const Argv&)
		 */
                ExecHelperOptions(const ExecHelperOptions& other) = delete;

		/*! @copydoc Argv::Argv(Argv&&)
		 */
                ExecHelperOptions(ExecHelperOptions&& other) noexcept = delete;
                ~ExecHelperOptions() override = default;

		/*! @copydoc Argv::operator=(const Argv&)
		 */
                ExecHelperOptions& operator=(const ExecHelperOptions& other) = delete;

		/*! @copydoc Argv::operator=(Argv&&)
		 */
                ExecHelperOptions& operator=(ExecHelperOptions&& other) = delete;

                /*! @copydoc Argv::operator==(const Argv&)
                 */
                bool operator==(const ExecHelperOptions& other) const noexcept;

                /*! @copydoc Argv::operator!=(const Argv&)
                 */
                bool operator!=(const ExecHelperOptions& other) const noexcept;

                /*! @copydoc Argv::swap(Argv&)
                 */
                void swap(ExecHelperOptions& other) noexcept;

                /**
                 * Returns the settings file that is used
                 *
                 * \param[in] argc  The number of arguments in argv
                 * \param[in] argv  The arguments passed on the command line
                 * \returns The relative or absolute path to the settings file
                 */
                std::string getSettingsFile(int argc, const char* const * argv) const noexcept;
                bool parse(int argc, const char* const * argv) override;
                bool parseSettingsFile(const config::Path& file) noexcept override;

                bool getVerbosity() const noexcept override;
                bool getDryRun() const noexcept override;
                bool getSingleThreaded() const noexcept override;
                boost::optional<log::LogLevel> getLogLevel() const noexcept override;
                const CommandCollection& getCommands() const noexcept override;
                const config::SettingsNode& getSettings() const noexcept override;
                const config::SettingsNode& getSettings(const std::string& key) noexcept override;
                const config::SettingsNode& getSettings(const std::string& key) const noexcept override;
                bool containsHelp() const noexcept override;
                bool contains(const std::string& longOptions) const noexcept override;
                std::vector<std::string> getLongOption(const std::string& longOptions) const noexcept override;
                const PatternsHandler& getPatternsHandler() const noexcept override;
                PatternValues getValues(const Pattern& pattern) const noexcept override;
                PatternPermutator makePatternPermutator(const PatternKeys& patterns) const noexcept override;

                /**
                 * Prints the help for these options
                 */
                void printHelp() const noexcept;

                void setExecutor(ExecutorInterface* executor) noexcept override;
                ExecutorInterface* getExecutor() const noexcept override;

            private:
                bool m_verbose{false};
                bool m_dryRun{false};
                bool m_singleThreaded{false};
                CommandCollection m_commands;
                config::SettingsNode m_settings;
                boost::program_options::variables_map m_optionsMap;
                PatternsHandler m_patternsHandler;
                execHelper::core::OptionDescriptions m_optionsDescriptions;

                ExecutorInterface* m_executor{nullptr};       // Non-owning pointer
        };
    } // namespace core
} // namespace execHelper

#endif /* __EXEC_HELPER_OPTIONS_H__ */
