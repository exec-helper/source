#ifndef __CLANG_TIDY_H__
#define __CLANG_TIDY_H__

#include "core/task.h"

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        class ClangTidy : public Plugin
        {
            public:
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;

            private:
                static std::string getChecks(const core::TaskCollection& checksCollection) noexcept;
        };
    }
}

#endif  /* __CLANG_TIDY_H__ */
