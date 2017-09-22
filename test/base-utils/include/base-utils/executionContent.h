#ifndef EXECUTION_CONTENT_INCLUDE
#define EXECUTION_CONTENT_INCLUDE

#include <string>

#include "base-utils/tmpFile.h"

namespace execHelper {
    namespace test {
        namespace baseUtils {
            class ExecutionContent
            {
                public:
                    using ConfigCommand = std::vector<std::string>;

                    ExecutionContent() noexcept;
                    ExecutionContent(const ExecutionContent& other) = delete;
                    ExecutionContent(ExecutionContent&& other) noexcept;

                    ExecutionContent& operator=(const ExecutionContent& other) = delete;
                    ExecutionContent& operator=(ExecutionContent&& other) noexcept;

                    void swap(ExecutionContent& other) noexcept;

                    void init() noexcept;

                    ConfigCommand getConfigCommand() const noexcept;
                    static std::string getIterationContent() noexcept;

                    std::string getPath() const noexcept;
                    unsigned int getNumberOfExecutions() const noexcept;

                    void clear() noexcept;

                private:
                    baseUtils::TmpFile m_file;
            };
            
        } // namespace utils
    } // namespace test
} // namespace execHelper

#endif  /* EXECUTION_CONTENT_INCLUDE */
