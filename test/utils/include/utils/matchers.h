#ifndef MATCHERS_INCLUDE
#define MATCHERS_INCLUDE

#include "plugins/memory.h"

#include "unittest/catch.h"

#include "executorStub.h"

namespace execHelper::test {
   class MatchExecution : public Catch::MatcherBase<plugins::SpecialMemory::Memories> {
       public:
            MatchExecution(const core::test::ExecutorStub::TaskQueue& tasks, const config::Patterns& patterns) noexcept;

            MatchExecution(const MatchExecution& other) = default;
            MatchExecution(MatchExecution&& other) = default;

            ~MatchExecution() noexcept override = default;

            auto operator=(const MatchExecution& other) -> MatchExecution& = delete;
            auto operator=(MatchExecution&& other) -> MatchExecution& = delete;

            auto match(const plugins::SpecialMemory::Memories& executions) const noexcept -> bool override;
            auto describe() const noexcept -> std::string override;

       private:
           const core::test::ExecutorStub::TaskQueue& m_tasks;
           const config::Patterns& m_patterns;
   };
} // namespace execHelper::test

#endif  /* MATCHERS_INCLUDE */
