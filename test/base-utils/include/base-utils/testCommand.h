#ifndef TEST_COMMAND_INCLUDE
#define TEST_COMMAND_INCLUDE

#include <memory>
#include <string>
#include <vector>

#include "statement.h"
#include "yaml.h"

namespace execHelper {
namespace test {
namespace baseUtils {
using CommandKey = std::string;
using CommandKeys = std::vector<CommandKey>;
using Statements = std::vector<std::shared_ptr<Statement>>;

class TestCommand {
  public:
    TestCommand(CommandKey commandKey,
                Statements initialStatements = {}) noexcept;

    std::shared_ptr<Statement> operator[](size_t index) const noexcept;

    Statements::const_iterator begin() const noexcept;
    Statements::const_iterator end() const noexcept;

    size_t size() const noexcept;
    std::string get() const noexcept;
    unsigned int getNbOfStatements() const noexcept;
    unsigned int getNumberOfStatementExecutions()
        const noexcept; // Returns the sum of executions of all statements
    void add(std::shared_ptr<Statement> statement) noexcept;
    void resetExecutions() noexcept;

    void write(YamlWriter& yaml) const noexcept;

  private:
    std::string m_command;
    Statements m_statements;
};
using Commands = std::vector<TestCommand>;

template <typename T, typename... Args>
inline std::shared_ptr<Statement> createStatement(Args... args) noexcept {
    return std::static_pointer_cast<Statement>(std::make_shared<T>(args...));
}
} // namespace baseUtils
} // namespace test
} // namespace execHelper

#endif /* TEST_COMMAND_INCLUDE */
