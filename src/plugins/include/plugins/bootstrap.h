#ifndef __BOOTSTRAP_H__
#define __BOOTSTRAP_H__

#include "buildPlugin.h"
#include "core/task.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }
    namespace core {
        class CompilerDescriptionElement;
    }
}

namespace execHelper {
    namespace plugins {
        class Bootstrap : public BuildPlugin {
            public:
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;

            private:
                static std::string getBootstrapFilename(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
        };
    }
}

#endif /* __BOOTSTRAP_H__ */
