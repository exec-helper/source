#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "core/options.h"

namespace execHelper {
    namespace core {
        class Task;
        class Options;
    }
}

namespace execHelper {
    namespace plugins {
        class Plugin {
            public: 
                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept = 0;

            protected:
                Plugin() {}
        };

        void registerTask(const core::Task& task, const core::Options& options) noexcept;
    }
}

#endif /* __PLUGIN_H__ */
