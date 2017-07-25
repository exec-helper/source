#ifndef __BOOTSTRAP_H__
#define __BOOTSTRAP_H__

#include "config/commandLineOptions.h"
#include "core/task.h"

#include "buildPlugin.h"

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
                std::string getPluginName() const noexcept override;
                config::VariablesMap getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions) const noexcept override;
                bool apply(core::Task task, const config::VariablesMap& variables, const config::Patterns& patterns) const noexcept override;

            private:
                /**
                 * Returns the bootstrap filename to use
                 *
                 * \param[in] command   The command to find the bootstrap filename for
                 * \param[in] rootSettings  The configuration settings associated with the specific command
                 * \returns The name of the bootstrap script to call
                 */
                static std::string getBootstrapFilename(const config::Command& command, const config::SettingsNode& rootSettings) noexcept;
        };
    } // namespace plugins
} // namespace execHelper

#endif /* __BOOTSTRAP_H__ */
