#ifndef __BOOTSTRAP_H__
#define __BOOTSTRAP_H__

#include "plugin.h"
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
        class Bootstrap : public Plugin {
            public:
                virtual bool apply(const std::string& command, core::Task& task, const core::Options& options) const noexcept override;

            private:
                bool init(core::Task& task, const core::Options& options) const noexcept;

                static core::TaskCollection getBuildDir(const config::SettingsNode& settings, const core::CompilerDescriptionElement& compiler) noexcept;
                static std::string getBootstrapFilename(const config::SettingsNode& settings) noexcept;
        };
    }
}

#endif /* __BOOTSTRAP_H__ */
