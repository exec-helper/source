#include "plugin.h"

#include "core/executorInterface.h"

#include "logger.h"

using execHelper::core::Task;

namespace {
void noExecuteCallback(const Task& /*task*/) noexcept {
    LOG(warning) << "Execute callback is called while no execute callback was "
                    "registered";
}

execHelper::plugins::ExecuteCallback& getExecuteCallback() noexcept {
    static execHelper::plugins::ExecuteCallback executeCallback(
        noExecuteCallback);
    return executeCallback;
}
} // namespace

namespace execHelper::plugins {
void registerExecuteCallback(const ExecuteCallback& callback) noexcept {
    getExecuteCallback() = callback;
}

bool registerTask(const Task& task) noexcept {
    getExecuteCallback()(task);
    return true;
}
} // namespace execHelper::plugins
