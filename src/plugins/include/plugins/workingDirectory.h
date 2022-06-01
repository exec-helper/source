#ifndef WORKING_DIRECTORY_INCLUDE
#define WORKING_DIRECTORY_INCLUDE

#include <string>

#include "config/commandLineOptions.h"
#include "config/variablesMap.h"
#include "core/task.h"

namespace execHelper::config {
class FleetingOptionsInterface;
} // namespace execHelper::config

namespace execHelper::plugins {
using WorkingDir = std::string;
using namespace std::literals;
constexpr auto WORKING_DIR_KEY = "working-dir"sv;

/**
 * \brief Extends the functionality to add the working directory configuration variables of the process to execute, using the _working-dir_ key in the configuration
 */
struct WorkingDirectory {
    /**
     * Adds the variables for this functionality to the given variables map
     *
     * @param[in] options       The fleeting options to take into account
     * @param[out] variables    The variables map to add the variables to
     */
    inline static void
    getVariables([[maybe_unused]] config::VariablesMap& variables,
                 [[maybe_unused]] const config::FleetingOptionsInterface&
                     options) noexcept {
        ;
    }

    /**
     * Applies the given variables to the task
     *
     * @param[in] variables The variables map to use
     * @param[out] task The task with the given variables map applied to it
     */
    inline static void apply(core::Task& task,
                             const config::VariablesMap& variables) noexcept {
        auto workingDir = variables.get<WorkingDir>(WORKING_DIR_KEY);
        if(workingDir) {
            task.setWorkingDirectory(*workingDir);
        }
    }
};
} // namespace execHelper::plugins

#endif /* WORKING_DIRECTORY_INCLUDE */
