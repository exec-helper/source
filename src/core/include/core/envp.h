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

                char** getEnvp() noexcept;

            private:
                using Envp_t = std::vector<char*>;

                void deepCopy(const Envp& other) noexcept;

                Envp_t m_envp;
        };
    } // core
} // execHelper

#endif  /* ENVP_INCLUDES */
