#ifndef __EXECUTE_PLUGIN_H__
#define __EXECUTE_PLUGIN_H__

#include <string>
#include <memory>

#include "plugin.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }
}

namespace execHelper {
    namespace plugins {
        class ExecutePlugin : public Plugin {
            public:
                ExecutePlugin(const config::SettingsNode& settings);
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;

                static std::shared_ptr<Plugin> getPlugin(const std::string& pluginName) noexcept;

            private:
                const config::SettingsNode& m_settings;
        };
    }
}

#endif  /* __EXECUTE_PLUGIN_H__ */
