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
        class Cppcheck : public Plugin {
            public:
                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;
            private:
                static std::string getSourceDir(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
                static std::string getEnabledChecks(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
        };
    }
}

#endif /* __CPPCHECK_H__ */
