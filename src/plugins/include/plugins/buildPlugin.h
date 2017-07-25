#ifndef __BUILD_PLUGIN_H__
#define __BUILD_PLUGIN_H__

#include <string>

#include "config/commandLineOptions.h"
#include "core/task.h"

#include "plugin.h"

namespace execHelper {
    namespace config {
        class SettingsNode;
    } // namespace config

    namespace core {
        class TargetDescriptionElement;
        class CompilerDescriptionElement;
        class Options;
    } // namespace core
} // namespace execHelper

namespace execHelper {
    namespace plugins {
        using BuildDir = std::string;

        /**
         * \brief Base class for all plugins related to building source code
         */
        class BuildPlugin : public Plugin {
            public:
                /*! @copydoc config::Argv::Argv(const Argv&)
                 */
                BuildPlugin(const BuildPlugin& other) = delete;

                /*! @copydoc config::Argv::Argv(Argv&&)
                 */
                BuildPlugin(BuildPlugin&& other) noexcept = delete;
                ~BuildPlugin() override = default;

                /*! @copydoc config::Argv::operator=(const Argv&)
                 */
                BuildPlugin& operator=(const BuildPlugin& other) = delete;

                /*! @copydoc config::Argv::operator=(Argv&&)
                 */
                BuildPlugin& operator=(BuildPlugin&& other) noexcept = delete;

                /**
                 * Returns the configuration key for setting the build directory for out of tree builds
                 *
                 * \returns The build directory configuration key
                 */
                static const std::string& getBuildDirKey() noexcept;

            protected:
                BuildPlugin() = default;
        };
    } // namespace plugins
} // namespace execHelper

#endif  /* __BUILD_PLUGIN_H__ */
