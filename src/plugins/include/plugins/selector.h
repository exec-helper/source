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
                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;
        };
    }
}

#endif  /* __SELECTOR_H__ */
