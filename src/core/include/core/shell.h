#ifndef __SHELL_INTERFACE_H__
#define __SHELL_INTERFACE_H__

#include <cstdint>

namespace execHelper {
    namespace core {
        class Task;
    }
}

namespace execHelper {
    namespace core {
        class Shell {
            public:
                typedef uint32_t ShellReturnCode;

                virtual ShellReturnCode execute(const Task& task) noexcept = 0;
                virtual bool isExecutedSuccessfully(ShellReturnCode returnCode) const noexcept = 0;

            protected:
                Shell() {};
        };
    }
}

#endif /* __SHELL_INTERFACE_H__ */
