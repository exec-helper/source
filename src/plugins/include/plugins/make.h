#ifndef __MAKE_H__
#define __MAKE_H__

#include "plugin.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }
    namespace core {
        class Task;
        class CompilerDescription;
    }
}

namespace execHelper {
    namespace plugins {
        class Make : public Plugin {
            public:
                virtual bool apply(const std::string& command, core::Task& task, const core::ExecHelperOptions& options) const noexcept;

            private:
                bool build(core::Task& task, const core::ExecHelperOptions& options) const noexcept;
                bool clean(core::Task& task, const core::ExecHelperOptions& options) const noexcept;

                static core::TaskCollection getBuildDir(const config::SettingsNode& settings, const core::CompilerDescription& compiler) noexcept;
                static core::TaskCollection getCommandLine(const config::SettingsNode& settings, const core::CompilerDescription& compiler) noexcept;
                static core::TaskCollection getMultiThreaded(const config::SettingsNode& settings) noexcept;
        };
    }
}

#endif /* __MAKE_H__ */
