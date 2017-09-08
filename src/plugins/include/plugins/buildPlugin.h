#ifndef __BUILD_PLUGIN_H__
#define __BUILD_PLUGIN_H__

#include "core/patternsHandler.h"
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
        /**
         * \brief Base class for all plugins related to building source code
         */
        class BuildPlugin : public Plugin {
            public:
		/*! @copydoc core::Argv::Argv(const Argv&)
		 */
                BuildPlugin(const BuildPlugin& other) = delete;

		/*! @copydoc core::Argv::Argv(Argv&&)
		 */
                BuildPlugin(BuildPlugin&& other) noexcept = delete;
                ~BuildPlugin() override = default;

		/*! @copydoc core::Argv::operator=(const Argv&)
		 */
                BuildPlugin& operator=(const BuildPlugin& other) = delete;

		/*! @copydoc core::Argv::operator=(Argv&&)
		 */
                BuildPlugin& operator=(BuildPlugin&& other) noexcept = delete;

                /**
                 * Returns the configuration key for setting the build directory for out of tree builds
                 *
                 * \returns The build directory configuration key
                 */
                static const std::string& getBuildDirKey() noexcept;

                /**
                 * Returns whether the multi-threaded option is set
                 *
                 * \param[in] command   The command to check the option for
                 * \param[in] rootSettings  The configuration settings associated with the specific command
                 * \param[in] options  The command line options
                 * \returns Whether the multi-threaded option is set
                 */
                static bool getMultiThreaded(const core::Command& command, const config::SettingsNode& rootSettings, const core::Options& options) noexcept;

            protected:
                BuildPlugin() = default;
        };
    } // namespace plugins
} // namespace execHelper

#endif  /* __BUILD_PLUGIN_H__ */
