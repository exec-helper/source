#ifndef __BUILD_PLUGIN_H__
#define __BUILD_PLUGIN_H__

#include "plugin.h"
#include "core/task.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }

    namespace core {
        class CompilerDescriptionElement;
        class Options;
    }
}

namespace execHelper {
    namespace plugins {
        class BuildPlugin : public Plugin {
            protected:
                BuildPlugin() {}

                static core::TaskCollection getCommandLine(const core::Command& command, const config::SettingsNode& settings, const core::CompilerDescriptionElement& compiler) noexcept;
                static core::TaskCollection getMultiThreaded(const core::Command& command, const config::SettingsNode& settings, const core::Options& options) noexcept;
                static core::TaskCollection getBuildDir(const core::Command& command, const config::SettingsNode& settings, const core::CompilerDescriptionElement& compiler) noexcept;
        };
    }
}

#endif  /* __BUILD_PLUGIN_H__ */
