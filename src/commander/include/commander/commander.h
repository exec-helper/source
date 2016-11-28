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
    namespace core {
        class Commander {
            public:
                Commander(const ExecHelperOptions& options);

                void run() noexcept;

            private:
                void executePlugin(const std::string& pluginName, const std::string& command, const ExecHelperOptions& options) noexcept;

                static std::shared_ptr<plugins::Plugin> getPlugin(const std::string& pluginName) noexcept;

                const ExecHelperOptions& m_options;
        };
    }
}

#endif  /* __COMMANDER_H__ */
