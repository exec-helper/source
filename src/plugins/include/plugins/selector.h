#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        class Selector : public Plugin {
            public:
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;
        };
    }
}

#endif  /* __SELECTOR_H__ */
