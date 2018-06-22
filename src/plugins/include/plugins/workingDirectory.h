#ifndef WORKING_DIRECTORY_INCLUDE
#define WORKING_DIRECTORY_INCLUDE

#include <string>

#include <gsl/string_span>

#include "config/commandLineOptions.h"
#include "config/variablesMap.h"
#include "core/task.h"

namespace execHelper {
namespace config {
class FleetingOptionsInterface;
} // namespace config
} // namespace execHelper

namespace execHelper {
namespace plugins {
using WorkingDir = std::string;
const gsl::czstring<> WORKING_DIR_KEY = "working-dir";

/**
 * \brief Extends the functionality to add the working directory configuration variables of the process to execute, using the _working-dir_ key in the configuration
 */
struct WorkingDirectory {
    /**
     * Adds the variables for this functionality to the given variables map
     */
    inline static void
    getVariables(config::VariablesMap& /*variables*/,
                 const config::FleetingOptionsInterface& /*options*/) noexcept {
        ;
    }

    /*! @copydoc JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    inline static void apply(core::Task& task,
                             const config::VariablesMap& variables) noexcept {
        auto workingDir = variables.get<WorkingDir>(WORKING_DIR_KEY);
        if(workingDir) {
            task.setWorkingDirectory(*workingDir);
        }
    }
};
} // namespace plugins
} // namespace execHelper

#endif /* WORKING_DIRECTORY_INCLUDE */
