#include "plugin.h"
#include "core/executorInterface.h"

using execHelper::core::Task;
using execHelper::core::Options;

namespace execHelper { namespace plugins {
    bool registerTask(const Task& task, const Options& options) noexcept {
        return options.getExecutor()->execute(task);
    }
} }
