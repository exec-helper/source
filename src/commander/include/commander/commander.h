#ifndef __COMMANDER_H__
#define __COMMANDER_H__

#include <string>
#include <memory>

namespace execHelper {
    namespace core {
        class ExecHelperOptions;
    }

    namespace plugins {
        class Plugin;
    }
}

namespace execHelper {
    namespace commander {
        class Commander {
            public:
                Commander(const core::ExecHelperOptions& options);

                bool run() noexcept;

            private:
                bool executePlugin(const std::string& pluginName, const std::string& command, const core::ExecHelperOptions& options) noexcept;

                static std::shared_ptr<plugins::Plugin> getPlugin(const std::string& pluginName) noexcept;

                const core::ExecHelperOptions& m_options;
        };
    }
}

#endif  /* __COMMANDER_H__ */
