#ifndef THREADEDNESS_INCLUDE
#define THREADEDNESS_INCLUDE

#include <string>

#include <gsl/string_span>

#include "config/commandLineOptions.h"
#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"
#include "core/task.h"

namespace execHelper {
namespace plugins {
using Jobs = config::Jobs_t;

const gsl::czstring<> JOBS_KEY = "jobs";

/**
 * \brief Extends the functionality to include the _jobs_ config parameter and processes this parameter, using the --jobs flag
 */
struct JobsLong {
    /**
     * Adds the variables for this functionality to the given variables map
     *
     * @param[in] options       The fleeting options to take into account
     * @param[out] variables    The variables map to add the variables to
     */
    static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& options) noexcept;

    /**
     * Applies the given variables to the task
     *
     * @param[in] variables The variables map to use
     * @param[out] task The task with the given variables map applied to it
     */
    inline static void apply(core::Task& task,
                             const config::VariablesMap& variables) noexcept {
        task.append(
            {"--jobs", std::to_string(*(variables.get<Jobs>(JOBS_KEY)))});
    }
};

/**
 * \brief Extends the functionality to include the _jobs_ config parameter and processes this parameter, using the -j flag
 */
struct JobsShort {
    /*! @copydoc JobsLong::getVariables(config::VariablesMap&, const config::FleetingOptionsInterface&)
     */
    inline static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& options) noexcept {
        JobsLong::getVariables(variables, options);
    }

    /*! @copydoc JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    inline static void apply(core::Task& task,
                             const config::VariablesMap& variables) noexcept {
        task.append({"-j", std::to_string(*(variables.get<Jobs>(JOBS_KEY)))});
    }
};

} // namespace plugins
} // namespace execHelper

#endif /* THREADEDNESS_INCLUDE */
