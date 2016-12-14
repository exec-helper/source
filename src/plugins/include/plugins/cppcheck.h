#ifndef __CPPCHECK_H__
#define __CPPCHECK_H__

#include "plugin.h"
#include "core/task.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }
    namespace core {
        class TargetDescriptionElement;
    }
}

namespace execHelper {
    namespace plugins {
        class Cppcheck : public Plugin {
            public:
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;
            private:
                static core::TaskCollection getSourceDir(const core::Command& command, const config::SettingsNode& rootSettings, const core::TargetDescriptionElement& target) noexcept;
                static core::TaskCollection getEnabledChecks(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
        };
    }
}

#endif /* __CPPCHECK_H__ */
