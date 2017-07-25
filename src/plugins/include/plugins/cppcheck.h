#ifndef __CPPCHECK_H__
#define __CPPCHECK_H__

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        /**
         * \brief Plugin for executing the cppcheck statis analysis tool
         */
        class Cppcheck : public Plugin {
            public:
                std::string getPluginName() const noexcept override;
                config::VariablesMap getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions) const noexcept override;
                bool apply(core::Task task, const config::VariablesMap& variables, const config::Patterns& patterns) const noexcept override;
            private:
                /**
                 * Returns the source dir to use for the analysis
                 *
                 * \param[in] command   The command for which to get the source directory
                 * \param[in] rootSettings  The configuration settings associated with the specific command
                 * \returns The source dir to use
                 */
                static std::string getSourceDir(const config::Command& command, const config::SettingsNode& rootSettings) noexcept;

                /**
                 * Returns the checks to use for the analysis
                 *
                 * \param[in] variables   The configuration for this plugin
                 * \returns The checks that are enabled for the analysis
                 */
                static std::string getEnabledChecks(const config::VariablesMap& variables) noexcept;
        };
    }
}

#endif /* __CPPCHECK_H__ */
