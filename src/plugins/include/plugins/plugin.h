#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "core/execHelperOptions.h"
#include "core/task.h"
#include "core/executorInterface.h"

namespace execHelper {
    namespace plugins {
        class Plugin {
            public: 
                virtual void apply(core::Task& task, const core::ExecHelperOptions& options) const noexcept = 0;

            protected:
                Plugin() {}
        };

        void registerTask(const core::Task& task, const core::ExecHelperOptions& options) noexcept;
    }
}

#endif /* __PLUGIN_H__ */
