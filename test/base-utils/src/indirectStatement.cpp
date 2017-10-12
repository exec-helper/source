#include "indirectStatement.h"

using std::shared_ptr;

using gsl::not_null;

namespace execHelper {
    namespace test {
        namespace baseUtils {
            IndirectStatement::IndirectStatement(const StatementKey& key, const Statements& initialStatements) :
                Statement(SUCCESS),
                m_key(key),
                m_statements(initialStatements)
            {
                ;
            }

            void IndirectStatement::add(shared_ptr<Statement> statement) noexcept {
                m_statements.emplace_back(statement);
            }

            unsigned int IndirectStatement::getNumberOfExecutions() const noexcept {
                return accumulate(m_statements.begin(), m_statements.end(), 0U,
                        [](const unsigned int& subTotal, const std::shared_ptr<Statement>& statement) { return subTotal + statement->getNumberOfExecutions(); });
            };

            void IndirectStatement::resetExecutions() noexcept {
                for(auto& statement : m_statements) {
                    statement->resetExecutions();
                }
            }

            StatementKey IndirectStatement::getKey() const noexcept {
                return m_key;
            }

            void IndirectStatement::write(not_null<YamlWriter*> yaml, const std::string& /*command*/) const noexcept {
                if(! (*yaml)[m_key]) {
                    for(const auto& statement : m_statements) {
                        (*yaml)[m_key].push_back(statement->getKey());
                        statement->write(yaml, m_key);
                    }
                }
            };
        } // namespace baseUtils
    } // namespace test
} // namespace execHelper
