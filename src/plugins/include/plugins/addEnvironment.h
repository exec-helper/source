#ifndef ADD_ENVIRONMENT_INCLUDE
#define ADD_ENVIRONMENT_INCLUDE

#include "config/environment.h"
#include "config/fleetingOptionsInterface.h"
#include "config/path.h"
#include "config/variablesMap.h"
#include "core/task.h"

#include "pluginUtils.h"

namespace execHelper {
namespace plugins {
/**
 * \brief Extends the functionality to add environment variables to the environment of the process to execute, using the _environment_ key in the configuration
 */
struct AddEnvironment {
    /**
     * Adds the variables for this functionality to the given variables map
     *
     * @param[out] variables    The variables map to add the variables to
     */
    inline static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& /*options*/) noexcept {
        variables.add(config::ENVIRONMENT_KEY, config::EnvArgs());
    }

    /*! @copydoc JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    inline static void apply(core::Task& task,
                             const config::VariablesMap& variables) noexcept {
        task.appendToEnvironment(getEnvironment(variables));
    }
};
} // namespace plugins
} // namespace execHelper

#endif /* ADD_ENVIRONMENT_INCLUDE */
