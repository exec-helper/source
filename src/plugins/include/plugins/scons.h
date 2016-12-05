#ifndef __SCONS_H__
#define __SCONS_H__

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
        class Scons : public Plugin {
            public:
                virtual bool apply(const std::string& command, core::Task& task, const core::Options& options) const noexcept override;

            private:
                bool build(core::Task& task, const core::Options& options) const noexcept;
                bool clean(core::Task& task, const core::Options& options) const noexcept;

                static core::TaskCollection getCommandLine(const config::SettingsNode& settings, const core::CompilerDescriptionElement& compiler) noexcept;
                static core::TaskCollection getMultiThreaded(const config::SettingsNode& settings) noexcept;
        };
    }
}

#endif /* __SCONS_H__ */
