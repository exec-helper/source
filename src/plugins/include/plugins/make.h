#ifndef __MAKE_H__
#define __MAKE_H__

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
 * \brief Provides details for the make plugin
 */
struct MakeDetails {
    /*! @copydoc NinjaDetails::getName()
     */
    inline static std::string getName() { return "make"; }

    /*! @copydoc NinjaDetails::getBinary()
     */
    inline static std::string getBinary() { return "make"; }
};

using Make = PluginStrategy<MakeDetails, BuildDirLong, JobsLong, VerbosityDebug,
                            CommandLine, WorkingDirectory, AddEnvironment>;
} // namespace plugins
} // namespace execHelper

#endif /* __MAKE_H__ */
