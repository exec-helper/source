#ifndef __CLANG_STATIC_ANALYZER_H__
#define __CLANG_STATIC_ANALYZER_H__

#include <memory>

#include "buildPlugin.h"
#include "core/task.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }
    namespace core {
        class Options;
        class CompilerDescriptionElement;
    }
    namespace plugins {
        class Plugin;
    }
}

namespace execHelper {
    namespace plugins {
        class ClangStaticAnalyzer : public BuildPlugin {
            public:
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;
            private:
                static std::string getSystemName(const core::Command& command, const std::string& key, const config::SettingsNode& rootSettings) noexcept;
                static void getBuildOptions(core::Options& options, const core::Command& command, const core::CompilerDescriptionElement& compiler) noexcept;
                static void overrideBuildDir(const core::Command& command, core::Options& options, const std::string& key, const config::SettingsNode& rootSettings, const core::CompilerDescriptionElement& compiler) noexcept;
                static core::CompilerDescriptionElement getCompiler(const core::Command& command, const config::SettingsNode& rootSettings, const core::CompilerDescriptionElement& defaultElement) noexcept;
        };
    }
}

#endif /* __CLANG_STATIC_ANALYZER_H__ */
