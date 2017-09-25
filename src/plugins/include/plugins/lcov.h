#ifndef __LCOV_H__
#define __LCOV_H__

#include "core/task.h"

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        /**
         * \brief Plugin for running the lcov code coverage analysis tool
         */
        class Lcov : public Plugin {
            public:
                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;
            private:
                /**
                 * Add the command to generate html
                 *
                 * \param[in] command   The command associated with the action
                 * \param[in] rootSettings  The configuration settings associated with the specific command
                 * \param[in] infoFile  The infofile to use
                 * \param[in] task  The current task
                 * \returns True    if the command was successfully added to the given task
                 *          False   otherwise
                 */
                static bool generateGenHtmlTask(const core::Command& command, const config::SettingsNode& rootSettings, const std::string& infoFile, core::Task& task) noexcept;

                /**
                 * Add the command to zero all counters
                 *
                 * \param[in] command   The command associated with the action
                 * \param[in] rootSettings  The configuration settings associated with the specific command
                 * \param[in] baseDirectory The base directory to use. See lcov manpage
                 * \param[in] directory     The directory to use. See lcov manpage
                 * \param[in] commandLine   The additional command line arguments to add
                 * \param[in] task  The current task
                 * \returns True    if the command was successfully added to the given task
                 *          False   otherwise
                 */
                static bool generateZeroCountersTask(const core::Command& command, const config::SettingsNode& rootSettings, const std::string& baseDirectory, const std::string& directory, const core::TaskCollection& commandLine, core::Task& task) noexcept;

                /**
                 * Get the excludes
                 *
                 * \param[in] command   The command associated with the action
                 * \param[in] rootSettings  The configuration settings associated with the specific command
                 * \returns A collection of directories to exclude from the analysis
                 */
                static core::TaskCollection getExcludes(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;

                /**
                 * Add the command to exclude directories
                 *
                 * \param[in] command   The command associated with the action
                 * \param[in] rootSettings  The configuration settings associated with the specific command
                 * \param[in] infoFile      The info file associated with the analysis
                 * \param[in] commandLine   The additional command line arguments to add
                 * \param[in] task  The current task
                 * \returns True    if the command was successfully added to the given task
                 *          False   otherwise
                 */
                static bool generateExcludeTask(const core::Command& command, const config::SettingsNode& rootSettings, const std::string& infoFile, const core::TaskCollection& commandLine, core::Task& task) noexcept;

                /**
                 * Add the command to capture the coverage
                 *
                 * \param[in] baseDirectory The base directory to use. See lcov manpage
                 * \param[in] directory     The directory to use. See lcov manpage
                 * \param[in] infoFile      The info file associated with the analysis
                 * \param[in] commandLine   The additional command line arguments to add
                 * \param[in] task  The current task
                 */
                static void generateCaptureTask(const std::string& baseDirectory, const std::string& directory, const std::string& infoFile, const core::TaskCollection& commandLine, core::Task& task) noexcept;
        };
    }
}

#endif  /* __LCOV_H__ */
