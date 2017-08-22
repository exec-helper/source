#ifndef __CPPCHECK_H__
#define __CPPCHECK_H__

#include "core/patternsHandler.h"
#include "core/task.h"

#include "plugin.h"

namespace execHelper {
    namespace config {
        class SettingsNode;
    }
    namespace core {
        class TargetDescriptionElement;
    }
}

namespace execHelper {
    namespace plugins {
        /**
         * \brief Plugin for executing the cppcheck statis analysis tool
         */
        class Cppcheck : public Plugin {
            public:
                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;
            private:
                /**
                 * Returns the source dir to use for the analysis
                 *
                 * \param command   The command for which to get the source directory
                 * \param rootSettings  The configuration settings associated with the specific command
                 * \returns The source dir to use
                 */
                static std::string getSourceDir(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;

                /**
                 * Returns the checks to use for the analysis
                 *
                 * \param command   The command for which to get the enabled checks
                 * \param rootSettings  The configuration settings associated with the specific command
                 * \returns The checks that are enbled for the analysis
                 */
                static std::string getEnabledChecks(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
        };
    }
}

#endif /* __CPPCHECK_H__ */
