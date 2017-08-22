#ifndef __CLANG_TIDY_H__
#define __CLANG_TIDY_H__

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
                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;

            private:
                /**
                 * Returns the configured checks as a clang-tidy check command line argument
                 *
                 * @param checksCollection  The configured checks
                 * @returns The configured checks as a clang-tidy check command line argument
                 */
                static std::string getChecks(const core::TaskCollection& checksCollection) noexcept;

                /**
                 * Returns the configured warning-as-errors as a clang-tidy check command line argument
                 *
                 * @param warningAsErrorCollection  The configured warning-as-errors
                 * @param checksCollection  The configured checks
                 * @returns The configured warning-as-errors as a clang-tidy check command line argument
                 */
                static std::string getWarningAsError(const core::TaskCollection& warningAsErrorCollection, const core::TaskCollection& checksCollection) noexcept;

                /*
                 * Returns the given collection as clang-tidy command line style string
                 *
                 * @param checks  The collection to list
                 * @returns The configured collection as a clang-tidy command line style string
                 */
                static std::string listChecks(const core::TaskCollection& checks) noexcept;
        };
    }
}

#endif  /* __CLANG_TIDY_H__ */
