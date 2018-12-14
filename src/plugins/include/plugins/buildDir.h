#ifndef BUILD_DIR_INCLUDE
#define BUILD_DIR_INCLUDE

#include "config/fleetingOptionsInterface.h"
#include "config/path.h"
#include "config/variablesMap.h"
#include "core/task.h"

#include "buildPlugin.h"

namespace execHelper {
namespace plugins {
/**
 * \brief Extends the functionality to include the _build-dir_ config parameter and processes this parameter, using the --directory flag
 */
struct BuildDirLong {
    /*! @copydoc AddEnvironment::getVariables(config::VariablesMap&, const config::FleetingOptionsInterface&)
     */
    inline static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& /*options*/) noexcept {
        variables.add(BuildPlugin::getBuildDirKey(), ".");
    }

    /*! @copydoc JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    inline static void apply(core::Task& task,
                             const config::VariablesMap& variables) noexcept {
        task.append({"--directory",
                     variables.get<config::Path>(BuildPlugin::getBuildDirKey())
                         ->native()});
    }
};

/**
 * \brief Extends the functionality to include the _build-dir_ config parameter and processes this parameter, using the -C flag
 */
struct BuildDirShort {
    /*! @copydoc JobsLong::getVariables(config::VariablesMap&, const config::FleetingOptionsInterface&)
     */
    inline static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& options) noexcept {
        BuildDirLong::getVariables(variables, options);
    }

    /*! @copydoc JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    inline static void apply(core::Task& task,
                             const config::VariablesMap& variables) noexcept {
        task.append(
            {"-C", variables.get<config::Path>(BuildPlugin::getBuildDirKey())
                       ->native()});
    }
};
} // namespace plugins
} // namespace execHelper

#endif /* BUILD_DIR_INCLUDE */
