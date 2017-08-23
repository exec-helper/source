#ifndef __BUILD_PLUGIN_H__
#define __BUILD_PLUGIN_H__

#include "core/patternsHandler.h"
#include "core/task.h"

#include "plugin.h"

namespace execHelper {
    namespace config {
        class SettingsNode;
    } // namespace config

    namespace core {
        class TargetDescriptionElement;
        class CompilerDescriptionElement;
        class Options;
    } // namespace core
} // namespace execHelper

namespace execHelper {
    namespace plugins {
        class BuildPlugin : public Plugin {
            public:
                BuildPlugin(const BuildPlugin& other) = delete;
                BuildPlugin(BuildPlugin&& other) noexcept = delete;
                ~BuildPlugin() override = default;

                BuildPlugin& operator=(const BuildPlugin& other) = delete;
                BuildPlugin& operator=(BuildPlugin&& other) noexcept = delete;

                static const std::string& getBuildDirKey() noexcept;
                static bool getMultiThreaded(const core::Command& command, const config::SettingsNode& rootSettings, const core::Options& options) noexcept;

            protected:
                BuildPlugin() = default;
        };
    } // namespace plugins
} // namespace execHelper

#endif  /* __BUILD_PLUGIN_H__ */
