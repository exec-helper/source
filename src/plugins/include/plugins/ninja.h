#ifndef NINJA_INCLUDE
#define NINJA_INCLUDE

#include "addEnvironment.h"
#include "buildDir.h"
#include "commandLine.h"
#include "pluginStrategy.h"
#include "threadedness.h"
#include "verbosity.h"
#include "workingDirectory.h"

namespace execHelper {
namespace plugins {
/**
 * \brief Details for the ninja plugin
 */
struct NinjaDetails {
    /**
     * Returns the name of the plugin as identified in the configuration
     *
     * @returns The name of the plugin
     */
    inline static std::string getName() noexcept { return "ninja"; }

    /**
     * Returns the command-line command associated with the plugin
     *
     * @returns The associated command-line command
     */
    inline static std::string getBinary() noexcept { return "ninja"; }
};

using Ninja =
    PluginStrategy<NinjaDetails, BuildDirShort, JobsShort, VerbosityShort,
                   CommandLine, WorkingDirectory, AddEnvironment>;
} // namespace plugins
} // namespace execHelper

#endif /* NINJA_INCLUDE */
