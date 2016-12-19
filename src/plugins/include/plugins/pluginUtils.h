#ifndef __PLUGIN_UTILS_H__
#define __PLUGIN_UTILS_H__

#include <string>
#include <memory>

#include "core/options.h"
#include "core/task.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }
    namespace core {
        class TargetDescriptionElement;
        class CompilerDescriptionElement;
    }
    namespace plugins {
        class Plugin;
    }
}

namespace execHelper {
    namespace plugins {
        const std::string& getPatternsKey() noexcept;
        const config::SettingsNode& getContainingSettings(const std::string& command, const config::SettingsNode& rootSettings, const std::string& key) noexcept;
        core::TaskCollection getCommandLine(const core::Command& command, const config::SettingsNode& settings) noexcept;
        core::TaskCollection getCommandLine(const core::Command& command, const config::SettingsNode& rootSettings, const core::TargetDescriptionElement& target) noexcept;
        core::TaskCollection getCommandLine(const core::Command& command, const config::SettingsNode& settings, const core::CompilerDescriptionElement& compiler) noexcept;

        std::shared_ptr<Plugin> getPlugin(const std::string& pluginName) noexcept;
    }
}

#endif  /* __PLUGIN_UTILS_H__ */
