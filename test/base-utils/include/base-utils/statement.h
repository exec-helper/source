#ifndef STATEMENT_INCLUDE
#define STATEMENT_INCLUDE

#include <string>

#include "executionContent.h"
#include "yaml.h"

namespace execHelper {
namespace test {
namespace baseUtils {
using StatementKey = std::string;
using StatementCollection = ExecutionContent::ConfigCommand;

class Statement {
  public:
    virtual ~Statement() = default;

    virtual StatementKey getKey() const noexcept = 0;
    virtual void write(YamlWriter& yaml,
                       const std::string& command) const noexcept = 0;

    virtual inline unsigned int getNumberOfExecutions() const noexcept {
        return m_execution.getNumberOfExecutions();
    }

    virtual inline void resetExecutions() noexcept { m_execution.clear(); }

  protected:
    Statement(ReturnCode returnCode) noexcept : m_execution(returnCode) {}

    ExecutionContent m_execution;
};
} // namespace baseUtils
} // namespace test
} // namespace execHelper

#endif /* STATEMENT_INCLUDE */
