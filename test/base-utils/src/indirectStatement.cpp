#include "indirectStatement.h"

#include <iostream>

using std::cerr;
using std::endl;
using std::shared_ptr;

using gsl::not_null;

namespace execHelper {
namespace test {
namespace baseUtils {
IndirectStatement::IndirectStatement(StatementKey key,
                                     Statements initialStatements)
    : Statement(SUCCESS),
      m_key(std::move(key)),
      m_statements(std::move(initialStatements)) {
    ;
}

void IndirectStatement::add(shared_ptr<Statement> statement) noexcept {
    m_statements.emplace_back(statement);
}

unsigned int IndirectStatement::getNumberOfExecutions() const noexcept {
    return accumulate(m_statements.begin(), m_statements.end(), 0U,
                      [](const unsigned int& subTotal,
                         const std::shared_ptr<Statement>& statement) {
                          return subTotal + statement->getNumberOfExecutions();
                      });
}

void IndirectStatement::resetExecutions() noexcept {
    for(auto& statement : m_statements) {
        statement->resetExecutions();
    }
}

StatementKey IndirectStatement::getKey() const noexcept { return m_key; }

void IndirectStatement::write(not_null<YamlWriter*> yaml,
                              const std::string& /*command*/) const noexcept {
    try {
        if(!(*yaml)[m_key]) {
            for(const auto& statement : m_statements) {
                (*yaml)[m_key].push_back(statement->getKey());
                statement->write(yaml, m_key);
            }
        }
    } catch(const YAML::InvalidNode&) {
        cerr << "The given YAML node is invalid" << endl;
        assert(false);
    }
}
} // namespace baseUtils
} // namespace test
} // namespace execHelper
