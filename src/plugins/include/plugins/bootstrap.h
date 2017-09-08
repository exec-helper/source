#ifndef __BOOTSTRAP_H__
#define __BOOTSTRAP_H__

#include "buildPlugin.h"
#include "core/task.h"

namespace execHelper {
    namespace config {
        class SettingsNode;
    } // namespace config
    namespace core {
        class CompilerDescriptionElement;
    } // namespace core
} // namespace execHelper

namespace execHelper {
    namespace plugins {
        /**
         * \brief Plugin for running the typical bootstrap stage in a build process
         */
        class Bootstrap : public BuildPlugin {
            public:
                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;

            private:
                /**
                 * Returns the bootstrap filename to use
                 *
                 * \param[in] command   The command to find the bootstrap filename for
                 * \param[in] rootSettings  The configuration settings associated with the specific command
                 * \returns The name of the bootstrap script to call
                 */
                static std::string getBootstrapFilename(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
        };
    } // namespace plugins
} // namespace execHelper

#endif /* __BOOTSTRAP_H__ */
