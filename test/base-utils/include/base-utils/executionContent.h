#ifndef EXECUTION_CONTENT_INCLUDE
#define EXECUTION_CONTENT_INCLUDE

#include <string>

#include "base-utils/tmpFile.h"

namespace execHelper {
    namespace test {
        namespace baseUtils {
            using ReturnCode = int;

            class ExecutionContent
            {
                public:
                    using ConfigCommand = std::vector<std::string>;

                    ExecutionContent() noexcept;
                    ~ExecutionContent() = default;

                    ExecutionContent(const ExecutionContent& other) = delete;
                    ExecutionContent(ExecutionContent&& other) noexcept;

                    ExecutionContent& operator=(const ExecutionContent& other) = delete;
                    ExecutionContent& operator=(ExecutionContent&& other) noexcept;

                    void swap(ExecutionContent& other) noexcept;

                    void init() noexcept;

                    ConfigCommand getConfigCommand() const noexcept;
                    ConfigCommand getFailingConfigCommand(ReturnCode returnCode) const noexcept;
                    static std::string getIterationContent() noexcept;

                    std::string getPath() const noexcept;
                    unsigned int getNumberOfExecutions() const noexcept;

                    void clear() noexcept;

                private:
                    baseUtils::TmpFile m_file;
            };
            
        } // namespace baseUtils
    } // namespace test
} // namespace execHelper

#endif  /* EXECUTION_CONTENT_INCLUDE */
