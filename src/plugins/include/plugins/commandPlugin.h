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
                std::string getPluginName() const noexcept override;
                config::VariablesMap getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions) const noexcept override;
                bool apply(core::Task task, const config::VariablesMap& variables, const config::Patterns& patterns) const noexcept override;
        };
    }
}

#endif  /* __COMMAND_PLUGIN_H__ */
