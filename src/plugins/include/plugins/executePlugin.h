#ifndef __EXECUTE_PLUGIN_H__
#define __EXECUTE_PLUGIN_H__

#include <string>
#include <vector>
#include <memory>

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        class ExecutePlugin : public Plugin {
            public:
                ExecutePlugin(const std::vector<std::string>& commands) noexcept;
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;

                static std::unique_ptr<Plugin> getPlugin(const std::string& pluginName) noexcept;

            private:
                const std::vector<std::string> m_commands;
        };
    }
}

#endif  /* __EXECUTE_PLUGIN_H__ */
