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
                 * \param[in] task  The task to take the arguments from
                 */
                explicit Argv(const TaskCollection& task) noexcept;

                /**
                * Copy constructor
                *
                * \param[in] other The other object to copy from
                */
                Argv(const Argv& other) noexcept;

                /**
                 * Move constructor
                 *
                 * \param[in] other The other object to copy from
                 */
                Argv(Argv&& other) noexcept;
                ~Argv() noexcept;

                /**
                 * Copy assignment operator
                 *
                 * \param[in] other The other object to assign from
                 * \returns A reference to this object
                 */
                Argv& operator=(const Argv& other) noexcept;

                /**
                 * Move assignment operator
                 *
                 * \param[in] other The other object to assign from
                 * \returns A reference to this object
                 */
                Argv& operator=(Argv&& other) noexcept;

                /**
                 * Equality operator
                 *
                 * \param[in] other The other object to compare with
                 * \returns True    If the objects are considered equal
                 *          False   Otherwise
                 */
                bool operator==(const Argv& other) noexcept;

                /**
                 * Inequality operator
                 *
                 * \param[in] other The other object to compare with
                 * \returns !operator==(other)
                 */
                bool operator!=(const Argv& other) noexcept;

                /**
                 * Access operator
                 *
                 * \param[in] index The index of the element to access
                 * \returns A C-style string
                 */
                char* operator[](size_t index) const noexcept;

                /**
                 * Swap contents
                 *
                 * \param[in] other The other object to swap with
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

                /*! @copydoc getArgv()
                 */
                const char* const * getArgv() const noexcept;

            private:
                using Argv_t = std::vector<char*>;

                /**
                 * Create a deep copy
                 *
                 * \param[in] other The other object to copy
                 */
                void deepCopy(const Argv& other) noexcept;

                Argv_t m_argv;
        };

        /**
         * Streaming operator
         *
         * \param[in] os    The stream to stream to
         * \param[in] argv  The object to stream
         * \returns The stream that was streamed to
         */
        std::ostream& operator<<(std::ostream& os, const Argv& argv) noexcept;
    } // namespace core
} // namespace execHelper

#endif  /* __ARGV_H__ */
