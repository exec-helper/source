#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <tuple>
#include <vector>

#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/task.h"

#include "plugin.h"

namespace execHelper {
namespace plugins {
/**
 * \brief   Remembers information that was passed to it
 */
struct Memory_t {
    const core::Task task;                //!< The task to remember
    const config::VariablesMap variables; //!< The variables to remember
    const config::Patterns patterns;      //!< The patterns to remember

    /**
     * Constructor
     *
     * \param[in] aTask     The task to remember
     * \param[in] variables The variables to remember
     * \param[in] patterns  The patterns to remember
     */
    Memory_t(core::Task aTask, config::VariablesMap variables,
             // cppcheck-suppress passedByValue symbolName=patterns
             config::Patterns patterns)
        : task(std::move(aTask)),
          variables(std::move(variables)),
          patterns(patterns) {
        ;
    }
};

/**
 * \brief   Plugin for remembering later on what has been executed. Mainly
 * useful for testing purposes.
 */
class Memory : public Plugin {
  public:
    using Memories = std::vector<Memory_t>; //!< brief A collection of memories

    Memory() = default;

    Memory(const Memory& other) = delete;
    Memory(Memory&& other) noexcept = delete;

    ~Memory() override = default;

    Memory& operator=(const Memory& other) = delete;
    Memory& operator=(Memory&& other) noexcept = delete;

    config::VariablesMap
    getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions)
        const noexcept override;
    bool apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept override;

    std::string summary() const noexcept override;

  protected:
    /**
     * Getter for the executions that were remembered
     *
     * \returns A collection of memories
     */
    static const Memories& getExecutions() noexcept;

    /**
     * Reset the remembered memories
     */
    static void reset() noexcept;

    /**
     * Set the return code for the next invocation(s) of a Memory object
     *
     * \param[in]   returnCode  The code to return on the next invocation(s)
     */
    static void setReturnCode(bool returnCode) noexcept;

  private:
    static bool m_returnCode;
    static Memories m_executions;
};

/**
 * \brief Makes access to a memory more accessible
 */
class MemoryHandler : public Memory {
  public:
    using Memories = Memory::Memories; //!< brief A collection of memories

    MemoryHandler();

    MemoryHandler(const MemoryHandler& other) = delete;
    MemoryHandler(MemoryHandler&& other) noexcept = delete;

    ~MemoryHandler() override;

    MemoryHandler& operator=(const MemoryHandler& other) = delete;
    MemoryHandler& operator=(MemoryHandler&& other) noexcept = delete;

    /*! @copydoc Memory::getExecutions()
     */
    static const Memories& getExecutions() noexcept;

    /*! @copydoc Memory::reset()
     */
    static void reset() noexcept;

    /*! @copydoc Memory::setReturnCode(bool)
     */
    static void setReturnCode(bool returnCode) noexcept;
};

/**
 * \brief   Plugin for remembering later on what has been executed. Mainly
 * useful for testing purposes.
 */
class SpecialMemory : public Plugin {
  public:
    using Memories = std::vector<Memory_t>; //!< brief A collection of memories

    SpecialMemory() : SpecialMemory(true) {}

    /**
     * \param[in]   returnCode  The code to return on invocation
     */
    explicit SpecialMemory(bool returnCode) : SpecialMemory(returnCode, 0U) {}

    /**
     * \param[in]   returnCode  The code to return on invocation
     * \param[in]   expectedSize    The expected size of the memory. Passing this allows for some memory optimization.
     */
    SpecialMemory(bool returnCode, size_t expectedSize) noexcept;

    auto getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const noexcept
        -> config::VariablesMap override;
    auto apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept
        -> bool override;

    auto summary() const noexcept -> std::string override;

    /**
     * Getter for the executions that were remembered
     *
     * \returns A collection of memories
     */
    auto getExecutions() noexcept -> const Memories&;

  private:
    const bool m_returnCode;
    mutable Memories
        m_executions; // We are keeping state in this plugin, while this is not allowed in 'regular' plugins
};
} // namespace plugins
} // namespace execHelper

#endif /* __MEMORY_H__ */
