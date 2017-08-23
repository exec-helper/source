#ifndef ENVP_INCLUDES
#define ENVP_INCLUDES

#include "task.h"

namespace execHelper {
    namespace core {
        class Envp {
            public:
                explicit Envp(const EnvironmentCollection& env) noexcept;
                Envp(const Envp& other) noexcept;
                Envp(Envp&& other) noexcept;
                ~Envp() noexcept;

                Envp& operator=(const Envp& other) noexcept;
                Envp& operator=(Envp&& other) noexcept;

                void swap(Envp& other) noexcept;

                size_t size() const noexcept;
                char** getEnvp() noexcept;
                const char* const * getEnvp() const noexcept;

            private:
                using Envp_t = std::vector<char*>;

                void deepCopy(const Envp& other) noexcept;

                Envp_t m_envp;
        };

        std::ostream& operator<<(std::ostream& os, const Envp& argv) noexcept;
    } // namespace core
} // namespace execHelper

#endif  /* ENVP_INCLUDES */
