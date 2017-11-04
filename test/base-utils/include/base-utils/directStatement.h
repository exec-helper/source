#ifndef DIRECT_STATEMENT_INCLUDE
#define DIRECT_STATEMENT_INCLUDE

#include <iostream>

#include "plugins.h"

#include "yaml.h"

namespace execHelper {
namespace test {
namespace baseUtils {
template <typename Plugin> class DirectStatement : public Statement {
  public:
    DirectStatement(ReturnCode returnCode) : Statement(returnCode) { ; }

    virtual ~DirectStatement() = default;

    StatementKey getKey() const noexcept override { return Plugin::getKey(); }

    void write(gsl::not_null<YamlWriter*> yaml,
               const std::string& command) const noexcept override {
        try {
            (*yaml)[getKey()][command][getStatementKey()] = getStatement();
        } catch(const YAML::InvalidNode&) {
            std::cerr << "Somehow the used YAML node is invalid" << std::endl;
            assert(false);
        }
    };

  private:
    StatementCollection getStatement() const noexcept {
        return m_execution.getConfigCommand();
    };

    std::string getStatementKey() const noexcept {
        return Plugin::getStatementKey();
    }
};
using SimpleStatement = DirectStatement<CommandLineCommandSettings>;
} // namespace baseUtils
} // namespace test
} // namespace execHelper

#endif /* DIRECT_STATEMENT_INCLUDE */
