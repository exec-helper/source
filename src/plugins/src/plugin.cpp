#include "plugin.h"

using execHelper::core::Task;
using execHelper::core::ExecHelperOptions;

namespace execHelper { namespace plugins {
    void registerTask(const Task& task, const ExecHelperOptions& options) noexcept {
        options.getExecutor()->execute(task);
    }
} }
