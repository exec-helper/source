#ifndef EXECUTION_CONTENT_INCLUDE
#define EXECUTION_CONTENT_INCLUDE

#include <string>

#include "tmpFile.h"

namespace execHelper {
    namespace test {
        namespace utils {
            class ExecutionContent
            {
                public:
                    ExecutionContent() noexcept;
                    ExecutionContent(const ExecutionContent& other) = delete;
                    ExecutionContent(ExecutionContent&& other) noexcept;

                    ExecutionContent& operator=(const ExecutionContent& other) = delete;
                    ExecutionContent& operator=(ExecutionContent&& other) noexcept;

                    void swap(ExecutionContent& other) noexcept;

                    static std::string getIterationContent() noexcept;

                    std::string getPath() noexcept;
                    unsigned int getNumberOfExecutions() noexcept;

                private:
                    TmpFile m_file;
            };
            
        } // namespace utils
    } // namespace test
} // namespace execHelper

#endif  /* EXECUTION_CONTENT_INCLUDE */
