#include "reportingExecutor.h"

#include <iostream>

#include "log/log.h"

#include "shell.h"
#include "task.h"

namespace execHelper {
namespace core {
ReportingExecutor::ReportingExecutor() noexcept { ; }

void ReportingExecutor::execute(const Task& task) noexcept {
    user_feedback_info("Executing " << task.toString());
}
} // namespace core
} // namespace execHelper
