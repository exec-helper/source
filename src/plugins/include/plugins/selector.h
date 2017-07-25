#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        /**
         * \brief   Selects an execution path based on the content of a parameter or pattern
         */
        class Selector : public Plugin {
            public:
                std::string getPluginName() const noexcept override;
                config::VariablesMap getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions) const noexcept override;
                bool apply(core::Task task, const config::VariablesMap& variables, const config::Patterns& patterns) const noexcept override;
        };
    }
}

#endif  /* __SELECTOR_H__ */
