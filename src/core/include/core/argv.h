#ifndef __ARGV_H__
#define __ARGV_H__

#include <cstddef>
#include <vector>

#include "task.h"

namespace execHelper {
    namespace core {
        class Argv {
            public:
                explicit Argv(const TaskCollection& task) noexcept;
                Argv(const Argv& other) noexcept;
                Argv(Argv&& other) noexcept;
                ~Argv() noexcept;

                Argv& operator=(const Argv& other) noexcept;
                Argv& operator=(Argv&& other) noexcept;
                char* operator[](size_t index) const noexcept;

                void swap(Argv& other)  noexcept;

                size_t getArgc() const noexcept;
                char** getArgv() noexcept;

            private:
                using Argv_t = std::vector<char*>;

                void deepCopy(const Argv& other) noexcept;

                Argv_t m_argv;
        };
    } /* core */
} /* execHelper */

#endif  /* __ARGV_H__ */
