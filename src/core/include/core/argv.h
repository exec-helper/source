#ifndef __ARGV_H__
#define __ARGV_H__

#include <cstddef>
#include <vector>

#include "task.h"

namespace execHelper {
    namespace core {
        class Argv {
            public:
                typedef std::vector<char*> Argv_t;

                Argv(const TaskCollection& task) noexcept;
                Argv(const Argv& other) noexcept;
                ~Argv() noexcept;

                Argv& operator=(const Argv& other) noexcept;
                char* operator[](size_t index) const noexcept;

                size_t getArgc() const noexcept;
                char** getArgv() noexcept;

            private:
                void deepCopy(const Argv& other) noexcept;

                Argv_t m_argv;
        };
    } /* core */
} /* execHelper */

#endif  /* __ARGV_H__ */
