#ifndef __LCOV_H__
#define __LCOV_H__

#include "core/task.h"

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        class Lcov : public Plugin {
            public:
                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;
            private:
                static bool generateGenHtmlTask(const core::Command& command, const config::SettingsNode& rootSettings, const std::string& infoFile, core::Task& task) noexcept;
                static bool generateZeroCountersTask(const core::Command& command, const config::SettingsNode& rootSettings, const std::string& baseDirectory, const std::string& directory, const core::TaskCollection& commandLine, core::Task& task) noexcept;
                static core::TaskCollection getExcludes(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
                static bool generateExcludeTask(const core::Command& command, const config::SettingsNode& rootSettings, const std::string& infoFile, const core::TaskCollection& commandLine, core::Task& task) noexcept;
                static void generateCaptureTask(const std::string& baseDirectory, const std::string& directory, const std::string& infoFile, const core::TaskCollection& commandLine, core::Task& task) noexcept;
        };
    }
}

#endif  /* __LCOV_H__ */
