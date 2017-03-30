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
        class Scons : public BuildPlugin {
            public:
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;
        };
    }
}

#endif /* __SCONS_H__ */
