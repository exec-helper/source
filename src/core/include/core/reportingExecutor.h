#ifndef __REPORTING_EXECUTOR_H__
#define __REPORTING_EXECUTOR_H__

#include "executorInterface.h"

#include <functional>

#include "shell.h"

namespace execHelper {
namespace core {
class Task;
}
} // namespace execHelper

namespace execHelper {
namespace core {
/**
 * \brief Implements an executor that only reports what it is executing
 */
class ReportingExecutor : public ExecutorInterface {
  public:
    ReportingExecutor() noexcept;

    void execute(const Task& task) noexcept override;
};
} // namespace core
} // namespace execHelper
#endif /* __REPORTING_EXECUTOR_H__ */
