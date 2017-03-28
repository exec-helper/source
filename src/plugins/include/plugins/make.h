#ifndef __MAKE_H__
#define __MAKE_H__

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
        class Make : public BuildPlugin {
            public:
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;
        };
    }
}

#endif /* __MAKE_H__ */
