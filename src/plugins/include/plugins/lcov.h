#ifndef __LCOV_H__
#define __LCOV_H__

#include "config/path.h"
#include "core/task.h"

#include "commandLine.h"
#include "plugin.h"

namespace execHelper {
namespace plugins {
/**
 * \brief Plugin for running the lcov code coverage analysis tool
 */
class Lcov : public Plugin {
  public:
    config::VariablesMap
    getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions)
        const noexcept override;
    bool apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept override;

    std::string summary() const noexcept override;

  private:
    using BaseDir = config::Path;
    using Dir = config::Path;
    using InfoFile = config::Path;
    using Excludes = std::vector<std::string>;

    /**
     * Add the command to generate html
     *
     * \param[in] infoFile  The infofile to use
     * \param[in] variables The variables map
     * \param[in] task  The current task
     * \returns True    if the command was successfully added to the given task
     *          False   otherwise
     */
    static core::Task generateGenHtmlTask(const InfoFile& infoFile,
                                          const config::VariablesMap& variables,
                                          const core::Task& task) noexcept;

    /**
     * Add the command to zero all counters
     *
     * \param[in] baseDirectory The base directory to use. See lcov manpage
     * \param[in] directory     The directory to use. See lcov manpage
     * \param[in] commandLine   The additional command line arguments to add
     * \param[in] task  The current task
     * \returns True    if the command was successfully added to the given task
     *          False   otherwise
     */
    static core::Task
    generateZeroCountersTask(const BaseDir& baseDirectory, const Dir& directory,
                             const CommandLineArgs& commandLine,
                             const core::Task& task) noexcept;

    /**
     * Get the excludes
     *
     * \param[in] variables The variables map
     * \returns A collection of directories to exclude from the analysis
     */
    static Excludes getExcludes(const config::VariablesMap& variables) noexcept;

    /**
     * Add the command to exclude directories
     *
     * \param[in] variables The variables map
     * \param[in] infoFile      The info file associated with the analysis
     * \param[in] commandLine   The additional command line arguments to add
     * \param[in] task  The current task
     * \returns True    if the command was successfully added to the given task
     *          False   otherwise
     */
    static core::Task generateExcludeTask(const config::VariablesMap& variables,
                                          const InfoFile& infoFile,
                                          const CommandLineArgs& commandLine,
                                          const core::Task& task) noexcept;

    /**
     * Add the command to capture the coverage
     *
     * \param[in] baseDirectory The base directory to use. See lcov manpage
     * \param[in] directory     The directory to use. See lcov manpage
     * \param[in] infoFile      The info file associated with the analysis
     * \param[in] commandLine   The additional command line arguments to add
     * \param[in] task  The current task
     */
    static core::Task generateCaptureTask(const BaseDir& baseDirectory,
                                          const Dir& directory,
                                          const InfoFile& infoFile,
                                          const CommandLineArgs& commandLine,
                                          const core::Task& task) noexcept;
};
} // namespace plugins
} // namespace execHelper

#endif /* __LCOV_H__ */
