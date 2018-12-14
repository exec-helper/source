#ifndef VERBOSITY_INCLUDE
#define VERBOSITY_INCLUDE

#include <iostream>

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"
#include "core/task.h"

namespace execHelper {
namespace plugins {
using Verbosity = bool;

const gsl::czstring<> VERBOSITY_KEY = "verbose";

/**
 * \brief Extends the functionality to include the _verbose_ config parameter and processes this parameter, using the --verbose flag
 */
struct VerbosityLong {
    /*! @copydoc JobsLong::getVariables(config::VariablesMap&, const config::FleetingOptionsInterface&)
     */
    inline static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& options) noexcept {
        const auto verbosity = options.getVerbosity() ? "yes" : "no";
        variables.add(VERBOSITY_KEY, verbosity);
    }

    /*! @copydoc JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    inline static void apply(core::Task& task,
                             const config::VariablesMap& variables) noexcept {
        if(*(variables.get<Verbosity>(VERBOSITY_KEY))) {
            task.append("--verbose");
        }
    }
};

/**
 * \brief Extends the functionality to include the _verbose_ config parameter and processes this parameter, using the --debug flag
 */
struct VerbosityDebug {
    /*! @copydoc JobsLong::getVariables(config::VariablesMap&, const config::FleetingOptionsInterface&)
     */
    inline static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& options) noexcept {
        VerbosityLong::getVariables(variables, options);
    }

    /*! @copydoc JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    inline static void apply(core::Task& task,
                             const config::VariablesMap& variables) noexcept {
        if(*(variables.get<Verbosity>(VERBOSITY_KEY))) {
            task.append("--debug");
        }
    }
};

/**
 * \brief Extends the functionality to include the _verbose_ config parameter and processes this parameter, using the -v flag
 */
struct VerbosityShort {
    /*! @copydoc JobsLong::getVariables(config::VariablesMap&, const config::FleetingOptionsInterface&)
     */
    inline static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& options) noexcept {
        VerbosityLong::getVariables(variables, options);
    }

    /*! @copydoc JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    inline static void apply(core::Task& task,
                             const config::VariablesMap& variables) noexcept {
        if(*(variables.get<Verbosity>(VERBOSITY_KEY))) {
            task.append("-v");
        }
    }
};
} // namespace plugins
} // namespace execHelper

#endif /* VERBOSITY_INCLUDE */
