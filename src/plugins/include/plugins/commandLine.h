#ifndef COMMAND_LINE_INCLUDE
#define COMMAND_LINE_INCLUDE

#include <string>
#include <vector>

#include <gsl/string_span>

#include "config/fleetingOptionsInterface.h"
#include "config/variablesMap.h"
#include "core/task.h"

namespace execHelper {
namespace plugins {
using CommandLineArg = std::string;
using CommandLineArgs = std::vector<CommandLineArg>;

static const gsl::czstring<> COMMAND_LINE_KEY = "command-line";

/**
 * \brief Extends the functionality to include the _command-line_ config parameter and processes this parameter
 */
struct CommandLine {
    /**
     * Adds the variables for this functionality to the given variables map
     *
     * @param[out] variables    The variables map to add the variables to
     * @param[in] options       The fleeting options to consider
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
        task.append(*(variables.get<CommandLineArgs>(COMMAND_LINE_KEY)));
    }
};

} // namespace plugins
} // namespace execHelper

#endif /* COMMAND_LINE_INCLUDE */
