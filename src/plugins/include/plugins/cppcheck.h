#ifndef __CPPCHECK_H__
#define __CPPCHECK_H__

#include "plugin.h"
#include "core/task.h"
#include "core/patternsHandler.h"

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
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;
            private:
                static std::string getSourceDir(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
                static std::string getEnabledChecks(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
        };
    }
}

#endif /* __CPPCHECK_H__ */
