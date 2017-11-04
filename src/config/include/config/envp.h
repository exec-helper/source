#ifndef ENVP_INCLUDES
#define ENVP_INCLUDES

#include "environment.h"

namespace execHelper {
namespace config {
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
    Envp(const Envp& other) noexcept;

    /*! @copydoc Argv::Argv(Argv&&)
     */
    Envp(Envp&& other) noexcept;
    ~Envp() noexcept;

    /*! @copydoc Argv::operator=(const Argv&)
     */
    Envp& operator=(const Envp& other) noexcept;

    /*! @copydoc Argv::operator=(Argv&&)
     */
    Envp& operator=(Envp&& other) noexcept;

    /*! @copydoc Argv::swap(Argv&)
     */
    void swap(Envp& other) noexcept;

    /**
     * Returns the size of the collection
     *
     * \returns The size of the collection
     */
    size_t size() const noexcept;

    /**
     * Clears the current content of the collection
     */
    void clear() noexcept;

    /**
     * Get the envp pointer as an array of C-style strings. The array is
     * delimited by a nullptr.
     *
     * \returns A pointer to an array of pointers to environment variables
     */
    char** getEnvp() noexcept;

    /*! @copydoc getEnvp()
     */
    const char* const* getEnvp() const noexcept;

  private:
    using Envp_t = std::vector<char*>;

    /**
     * Create a deep copy
     *
     * \param[in] other The other object to copy
     */
    void deepCopy(const Envp& other) noexcept;

    Envp_t m_envp;
};

std::ostream& operator<<(std::ostream& os, const Envp& envp) noexcept;
} // namespace config
} // namespace execHelper

#endif /* ENVP_INCLUDES */
