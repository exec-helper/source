#ifndef __PMD_H__
#define __PMD_H__

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        /**
         * \brief Implements the PMD plugin
         */
        class Pmd : public Plugin {
            public:
                std::string getPluginName() const noexcept override;
                config::VariablesMap getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions) const noexcept override;
                bool apply(core::Task task, const config::VariablesMap& variables, const config::Patterns& patterns) const noexcept override;
        };
    }
}

#endif /* __PMD_H__ */
