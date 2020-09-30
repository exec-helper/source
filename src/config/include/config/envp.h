#ifndef ENVP_INCLUDES
#define ENVP_INCLUDES

#include "environment.h"

namespace execHelper::config {
/**
 * \brief Wrapper for the envp argument
 */
class Envp {
  public:
    /**
     * Create an object from the contents of an environment collection
     *
     * \param[in] env   The environment collection to take the content from
     */
    explicit Envp(const EnvironmentCollection& env) noexcept;

    /*! @copydoc Argv::Argv(const Argv&)
     */
    Envp(const Envp& other) noexcept = delete;

    /*! @copydoc Argv::Argv(Argv&&)
     */
    Envp(Envp&& other) noexcept = delete;
    ~Envp() noexcept;

    /*! @copydoc Argv::operator=(const Argv&)
     */
    auto operator=(const Envp& other) noexcept -> Envp& = delete;

    /*! @copydoc Argv::operator=(Argv&&)
     */
    auto operator=(Envp&& other) noexcept -> Envp& = delete;

    /**
     * Returns the size of the collection
     *
     * \returns The size of the collection
     */
    [[nodiscard]] auto size() const noexcept -> size_t;

    /**
     * Clears the current content of the collection
     */
    void clear() noexcept;

    /**
     * Get the envp pointer as an array of C-style strings. The array is
     * delimited by a nullptr.
     *
     * \returns A pointer to an array of pointers to environment variables
     * \note    Only remains valid while this whole object exists
     */
    [[nodiscard]] auto getEnvp() noexcept -> char**;

    /*! @copydoc getEnvp()
     */
    [[nodiscard]] auto getEnvp() const noexcept -> const char* const*;

  private:
    using Envp_t = std::vector<char*>;

    Envp_t m_envp;
};

auto operator<<(std::ostream& os, const Envp& envp) noexcept -> std::ostream&;
} // namespace execHelper::config

#endif /* ENVP_INCLUDES */
