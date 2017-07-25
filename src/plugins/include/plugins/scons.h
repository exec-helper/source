#ifndef __SCONS_H__
#define __SCONS_H__

#include "buildPlugin.h"
#include "core/task.h"

namespace execHelper {
    namespace config {
        class SettingsNode;
    }
    namespace core {
        class CompilerDescriptionElement;
    }
}

namespace execHelper {
    namespace plugins {
        /**
         * \brief   Plugin for running scons
         */
        class Scons : public BuildPlugin {
            public:
                std::string getPluginName() const noexcept override;
                config::VariablesMap getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions) const noexcept override;
                bool apply(core::Task task, const config::VariablesMap& variables, const config::Patterns& patterns) const noexcept override;
        };
    }
}

#endif /* __SCONS_H__ */
