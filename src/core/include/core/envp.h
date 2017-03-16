#ifndef ENVP_INCLUDES
#define ENVP_INCLUDES

#include "task.h"

namespace execHelper {
    namespace core {
        class Envp {
            public:
                Envp(const EnvironmentCollection& env) noexcept;
                Envp(const Envp& other) noexcept;
                ~Envp() noexcept;

                Envp& operator=(const Envp& other) noexcept;

                char** getEnvp() noexcept;

            private:
                typedef std::vector<char*> Envp_t;

                void deepCopy(const Envp& other) noexcept;

                Envp_t m_envp;
        };
    } // core
} // execHelper

#endif  /* ENVP_INCLUDES */
