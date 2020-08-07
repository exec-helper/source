#include "matchers.h"

#include <algorithm>
#include <utility>

#include "config/pattern.h"

#include "base-utils/logger.h"

#include "utils.h"

using std::all_of;

namespace execHelper::test {
MatchExecution::MatchExecution(const core::test::ExecutorStub::TaskQueue& tasks, const config::Patterns& patterns) noexcept :
    m_tasks(tasks),
    m_patterns(patterns)
    {
        ;
    }

auto MatchExecution::match(const plugins::SpecialMemory::Memories& executions ) const noexcept -> bool {
    if(executions.size() != m_tasks.size()) {
        return false;
    }

    auto task = m_tasks.begin();
    for(auto execution = executions.begin(); execution != executions.end(); ++execution, ++task) {
        if(execution->task != *task) {
            return false;
        }
    }

    return all_of(executions.begin(), executions.end(), [this](const auto& execution) {
        if(execution.patterns != m_patterns) {
            LOG(error) << execution.patterns << " do not match " << m_patterns << "!";
        }
        return execution.patterns == m_patterns;
    });
}

auto MatchExecution::describe() const noexcept -> std::string {
    return "does not match its expected tasks and patterns";
}
} // namespace execHelper::test
