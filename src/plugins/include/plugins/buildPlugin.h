#ifndef __BUILD_PLUGIN_H__
#define __BUILD_PLUGIN_H__

#include "plugin.h"
#include "core/task.h"
#include "core/patternsHandler.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }

    namespace core {
        class TargetDescriptionElement;
        class CompilerDescriptionElement;
        class Options;
    }
}

namespace execHelper {
    namespace plugins {
        class BuildPlugin : public Plugin {
            public:
                static const std::string& getBuildDirKey() noexcept;
                static bool getMultiThreaded(const core::Command& command, const config::SettingsNode& settings, const core::Options& options) noexcept;

            protected:
                BuildPlugin() {}
        };
    }
}

#endif  /* __BUILD_PLUGIN_H__ */
