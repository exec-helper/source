#ifndef __CLANG_STATIC_ANALYZER_H__
#define __CLANG_STATIC_ANALYZER_H__

#include <memory>

#include "buildPlugin.h"
#include "core/task.h"

namespace execHelper {
    namespace plugins {
        class ClangStaticAnalyzer : public BuildPlugin {
            public:
                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;
            private:
                static core::TaskCollection getSystemName(const core::Command& command, const std::string& key, const config::SettingsNode& rootSettings) noexcept;
        };
    }
}

#endif /* __CLANG_STATIC_ANALYZER_H__ */
