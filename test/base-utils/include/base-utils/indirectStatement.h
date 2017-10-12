#ifndef INDIRECT_STATEMENT_INCLUDE
#define INDIRECT_STATEMENT_INCLUDE

#include <memory>
#include <vector>

#include "statement.h"

namespace execHelper {
    namespace test {
        namespace baseUtils {
            using Statements = std::vector<std::shared_ptr<Statement>>;

            class IndirectStatement : public Statement {
                public:
                    IndirectStatement(const StatementKey& key, const Statements& initialStatements = {});
                    virtual ~IndirectStatement() = default;

                    void add(std::shared_ptr<Statement> statement) noexcept;
                    unsigned int getNumberOfExecutions() const noexcept override;
                    void resetExecutions() noexcept override;
                    StatementKey getKey() const noexcept override;
                    void write(gsl::not_null<YamlWriter*> yaml, const std::string& command) const noexcept override;

                private:
                    StatementKey m_key;
                    Statements m_statements;
            }; 

        } // namespace baseUtils
    } // namespace test
} // namespace execHelper

#endif  /* INDIRECT_STATEMENT_INCLUDE */
