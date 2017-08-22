#ifndef __COMMAND_PLUGIN_H__
#define __COMMAND_PLUGIN_H__

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        /**
         * \brief Plugin for executing the commands configured on the command line
         */
        class CommandPlugin : public Plugin {
            public:
                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;
        };
    }
}

#endif  /* __COMMAND_PLUGIN_H__ */
