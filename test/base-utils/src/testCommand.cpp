#include "testCommand.h"

#include <iostream>

#include "plugins.h"

using std::cerr;
using std::endl;
using std::shared_ptr;

using gsl::not_null;

namespace execHelper {
namespace test {
namespace baseUtils {
TestCommand::TestCommand(std::string commandKey,
                         Statements initialStatements) noexcept
    : m_command(std::move(commandKey)),
      m_statements(std::move(initialStatements)) {
    ;
}

shared_ptr<Statement> TestCommand::operator[](size_t index) const noexcept {
    return m_statements[index];
}

Statements::const_iterator TestCommand::begin() const noexcept {
    return m_statements.begin();
}

Statements::const_iterator TestCommand::end() const noexcept {
    return m_statements.end();
}

size_t TestCommand::size() const noexcept { return m_statements.size(); }

std::string TestCommand::get() const noexcept { return m_command; }

unsigned int TestCommand::getNbOfStatements() const noexcept {
    return m_statements.size();
}

unsigned int TestCommand::getNumberOfStatementExecutions() const noexcept {
    return accumulate(m_statements.begin(), m_statements.end(), 0U,
                      [](const unsigned int& subTotal,
                         const shared_ptr<Statement>& statement) {
                          return subTotal + statement->getNumberOfExecutions();
                      });
}

void TestCommand::add(std::shared_ptr<Statement> statement) noexcept {
    m_statements.emplace_back(statement);
}

void TestCommand::resetExecutions() noexcept {
    for(auto& statement : m_statements) {
        statement->resetExecutions();
    }
}

void TestCommand::write(not_null<YamlWriter*> yaml) const noexcept {
    try {
        (*yaml)["commands"][m_command] = "Execute the command";
        for(const auto& statement : m_statements) {
            (*yaml)[m_command].push_back(statement->getKey());
            statement->write(yaml, m_command);
        }
    } catch(const YAML::InvalidNode&) {
        cerr << "The given YAML node is invalid" << endl;
        assert(false);
    }
}
} // namespace baseUtils
} // namespace test
} // namespace execHelper
