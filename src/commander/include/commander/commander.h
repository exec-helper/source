#ifndef __COMMANDER_H__
#define __COMMANDER_H__

#include <string>
#include <map>
#include <memory>

#include "core/task.h"

namespace execHelper {
    namespace core {
        class Options;
    }

    namespace plugins {
        class Plugin;
    }
}

namespace execHelper {
    namespace commander {
        class Commander {
            public:
                Commander(const core::Options& options, core::EnvironmentCollection&& env = core::EnvironmentCollection());

                bool run() noexcept;
                
            private:
                bool executePlugin(const std::string& pluginName, const std::string& command, const core::Options& options) noexcept;

                const core::Options& m_options;
                const core::EnvironmentCollection m_env;
        };
    }
}

#endif  /* __COMMANDER_H__ */
