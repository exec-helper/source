#ifndef __CLANG_TIDY_H__
#define __CLANG_TIDY_H__

#include <string>
#include <vector>

#include "core/task.h"

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        /**
         * \brief Plugin for executing the clang-tidy static analyzer tool
         */
        class ClangTidy : public Plugin
        {
            public:
                std::string getPluginName() const noexcept override;
                config::VariablesMap getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions) const noexcept override;
                bool apply(core::Task task, const config::VariablesMap& variables, const config::Patterns& patterns) const noexcept override;

            private:
                using Checks = std::vector<std::string>;
                using WarningAsError = Checks;

                /**
                 * Returns the configured checks as a clang-tidy check command line argument
                 *
                 * @param[in] checks  The configured checks
                 * @returns The configured checks as a clang-tidy check command line argument
                 */
                static core::TaskCollection getChecks(const Checks& checks) noexcept;

                /**
                 * Returns the configured warning-as-errors as a clang-tidy check command line argument
                 *
                 * @param[in] warningAsError The configured warning-as-errors
                 * @param[in] checks The configured checks
                 * @returns The configured warning-as-errors as a clang-tidy check command line argument
                 */
                static core::TaskCollection getWarningAsError(const WarningAsError& warningAsError, const Checks& checks) noexcept;

                /*
                 * Returns the given collection as clang-tidy command line style string
                 *
                 * @param[in] checks  The collection to list
                 * @returns The configured collection as a clang-tidy command line style string
                 */
                static std::string listChecks(const Checks& checks) noexcept;
        };
    }
}

#endif  /* __CLANG_TIDY_H__ */
