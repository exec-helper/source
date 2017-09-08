#ifndef ENVP_INCLUDES
#define ENVP_INCLUDES

#include "task.h"

namespace execHelper {
    namespace core {
        /**
         * \brief Wrapper for the envp argument
         */
        class Envp {
            public:
                /**
                 * Create an object from the contents of an environment collection
                 *
                 * \param env   The environment collection to take the content from
                 */
                explicit Envp(const EnvironmentCollection& env) noexcept;

                /*! @copydoc Argv::Argv(const Argv& other)
                 */
                Envp(const Envp& other) noexcept;

                /*! @copydoc Argv::Argv(Argv&& other)
                 */
                Envp(Envp&& other) noexcept;
                ~Envp() noexcept;

                /*! @copydoc Argv::operator=(const Argv& other)
                 */
                Envp& operator=(const Envp& other) noexcept;

                /*! @copydoc Argv::operator=(Argv&& other)
                 */
                Envp& operator=(Envp&& other) noexcept;

                /**
                 * Swap contents
                 *
                 * \param other The other object to swap with
                 */
                void swap(Envp& other) noexcept;

                /**
                 * Returns the size of the collection
                 *
                 * \returns The size of the collection
                 */
                size_t size() const noexcept;

                /**
                 * Get the envp pointer as an array of C-style strings. The array is delimited by a nullptr.
                 *
                 * \returns A pointer to an array of pointers to environment variables
                 */
                char** getEnvp() noexcept;

                /*! @copydoc getEnvp()
                 */
                const char* const * getEnvp() const noexcept;

            private:
                using Envp_t = std::vector<char*>;

                /**
                 * Create a deep copy
                 *
                 * \param other The other object to copy
                 */
                void deepCopy(const Envp& other) noexcept;

                Envp_t m_envp;
        };

        std::ostream& operator<<(std::ostream& os, const Envp& argv) noexcept;
    } // namespace core
} // namespace execHelper

#endif  /* ENVP_INCLUDES */
