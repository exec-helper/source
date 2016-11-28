#ifndef __SCONS_H__
#define __SCONS_H__

#include "plugin.h"
#include "core/task.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }
    namespace core {
        class CompilerDescription;
    }
}

namespace execHelper {
    namespace plugins {
        class Scons : public Plugin {
            public:
                virtual bool apply(const std::string& command, core::Task& task, const core::ExecHelperOptions& options) const noexcept;

            private:
                bool build(core::Task& task, const core::ExecHelperOptions& options) const noexcept;
                bool clean(core::Task& task, const core::ExecHelperOptions& options) const noexcept;
                bool distclean(core::Task& task, const core::ExecHelperOptions& options) const noexcept;

                static core::TaskCollection getBuildDir(const config::SettingsNode& settings, const core::CompilerDescription& compiler) noexcept;
                static core::TaskCollection getCommandLine(const config::SettingsNode& settings, const core::CompilerDescription& compiler) noexcept;
                static core::TaskCollection getMultiThreaded(const config::SettingsNode& settings) noexcept;
        };
    }
}

#endif /* __SCONS_H__ */
