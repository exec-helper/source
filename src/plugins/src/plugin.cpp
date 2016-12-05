#include "plugin.h"
#include "core/executorInterface.h"

using execHelper::core::Task;
using execHelper::core::Options;

namespace execHelper { namespace plugins {
    void registerTask(const Task& task, const Options& options) noexcept {
        options.getExecutor()->execute(task);
    }
} }
