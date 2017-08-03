#ifndef __ARGV_H__
#define __ARGV_H__

#include <cstddef>
#include <vector>

#include "task.h"

namespace execHelper {
    namespace core {
        /**
         * \brief   Wraps the default arguments when calling other programs
         */
        class Argv {
            public:
                /**
                 * Convert task to the argv/argc structure
                 *
                 * \param task  The task to take the arguments from
                 */
                explicit Argv(const TaskCollection& task) noexcept;
                Argv(const Argv& other) noexcept;
                Argv(Argv&& other) noexcept;
                ~Argv() noexcept;

                Argv& operator=(const Argv& other) noexcept;
                Argv& operator=(Argv&& other) noexcept;

                /**
                 * Access operator
                 *
                 * \param index The index of the element to access
                 * \returns A C-style string
                 */

                char* operator[](size_t index) const noexcept;

                /**
                 * Swap contents
                 *
                 * \param other The other object to swap with
                 */
                void swap(Argv& other)  noexcept;

                /**
                 * Getter for the argc argument
                 *
                 * \returns The total number of arguments
                 */

                size_t getArgc() const noexcept;

                /**
                 * Getter for the argv argument
                 *
                 * \returns An array of pointers to C-style strings of length \ref getArgc()
                 */
                char** getArgv() noexcept;
                const char* const * getArgv() const noexcept;

            private:
                using Argv_t = std::vector<char*>;

                /**
                 * Create a deep copy
                 *
                 * \param other The other object to copy
                 */
                void deepCopy(const Argv& other) noexcept;

                Argv_t m_argv;
        };

        std::ostream& operator<<(std::ostream& os, const Argv& argv) noexcept;
    } // namespace core
} // namespace execHelper

#endif  /* __ARGV_H__ */
