#include "executionHandler.h"

#include <assert.h>

using std::move;
using std::string;

using gsl::not_null;

namespace execHelper {
    namespace test {
        namespace baseUtils {
            void ExecutionHandler::add(const string& key, ExecutionContent&& content) noexcept {
                m_outputs.emplace(key, move(content));
            }

            const ExecutionContent& ExecutionHandler::at(const string& key) const noexcept {
                assert(m_outputs.count(key) > 0U);
                return m_outputs.at(key);
            }

            ExecutionHandler::ExecutionHandlerIterationRAII ExecutionHandler::startIteration() noexcept {
                return ExecutionHandlerIterationRAII(&m_outputs);
            }

            ExecutionHandler::ExecutionHandlerIterationRAII::ExecutionHandlerIterationRAII(not_null<ExecutionContentCollection*> outputs) :
                m_outputs(outputs)
            {
                ;
            }

            ExecutionHandler::ExecutionHandlerIterationRAII::~ExecutionHandlerIterationRAII() {
                for(auto& output : *m_outputs) {
                    output.second.clear();
                }
            }
        } // namespace baseUtils
    } // namespace test
} // namespace execHelper
