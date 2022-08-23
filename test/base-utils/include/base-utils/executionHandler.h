#ifndef EXECUTION_HANDLER_INCLUDE
#define EXECUTION_HANDLER_INCLUDE

#include <map>

#include "executionContent.h"

namespace execHelper::test::baseUtils {
class ExecutionHandler {
  public:
    void add(const std::string& key, ExecutionContent&& content) noexcept;

    [[nodiscard]]
    auto at(const std::string& key) const noexcept -> const ExecutionContent&;

  private:
    using ExecutionContentCollection = std::map<std::string, ExecutionContent>;

    /**
     * \brief   Used for handling an execution iteration
     */
    class ExecutionHandlerIterationRAII {
      public:
        explicit ExecutionHandlerIterationRAII(
            ExecutionContentCollection& outputs);
        ~ExecutionHandlerIterationRAII();

        ExecutionHandlerIterationRAII(
            const ExecutionHandlerIterationRAII& other) = default;
        ExecutionHandlerIterationRAII(ExecutionHandlerIterationRAII&& other) =
            default;

        auto
        operator=(const ExecutionHandlerIterationRAII& other) = delete;
        auto
        operator=(ExecutionHandlerIterationRAII&& other) noexcept =
            delete;

        [[nodiscard]]
        auto at(const std::string& key) const noexcept -> const ExecutionContent&;

      private:
        ExecutionContentCollection& m_outputs;
    };

    ExecutionContentCollection m_outputs;

  public:
    ExecutionHandlerIterationRAII startIteration() noexcept;
};
} // namespace execHelper::test::baseUtils

#endif /* EXECUTION_HANDLER_INCLUDE */
