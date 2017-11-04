#ifndef EXECUTION_HANDLER_INCLUDE
#define EXECUTION_HANDLER_INCLUDE

#include <map>

#include <gsl/gsl>

#include "executionContent.h"

namespace execHelper {
namespace test {
namespace baseUtils {
class ExecutionHandler {
  public:
    void add(const std::string& key, ExecutionContent&& content) noexcept;

    const ExecutionContent& at(const std::string& key) const noexcept;

  private:
    using ExecutionContentCollection = std::map<std::string, ExecutionContent>;

    /**
     * \brief   Used for handling an execution iteration
     */
    class ExecutionHandlerIterationRAII {
      public:
        explicit ExecutionHandlerIterationRAII(
            gsl::not_null<ExecutionContentCollection*> outputs);
        ~ExecutionHandlerIterationRAII();

        ExecutionHandlerIterationRAII(
            const ExecutionHandlerIterationRAII& other) = default;
        ExecutionHandlerIterationRAII(ExecutionHandlerIterationRAII&& other) =
            default;

        ExecutionHandlerIterationRAII&
        operator=(const ExecutionHandlerIterationRAII& other) = default;
        ExecutionHandlerIterationRAII&
        operator=(ExecutionHandlerIterationRAII&& other) noexcept =
            default; // NOLINT(misc-noexcept-move-constructor)

        const ExecutionContent& at(const std::string& key) const noexcept;

      private:
        gsl::not_null<ExecutionContentCollection*> m_outputs;
    };

    ExecutionContentCollection m_outputs;

  public:
    ExecutionHandlerIterationRAII startIteration() noexcept;
};
} // namespace baseUtils
} // namespace test
} // namespace execHelper

#endif /* EXECUTION_HANDLER_INCLUDE */
