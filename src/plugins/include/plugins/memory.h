#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <tuple>
#include <vector>

#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/task.h"

#include "executionContext.h"
#include "plugin.h"

namespace execHelper::plugins {
/**
 * \brief   Plugin for remembering later on what has been executed. Mainly
 * useful for testing purposes.
 */
class Memory : public Plugin {
  public:
    using ApplyFunction = std::function<core::Tasks(
        core::Task, const config::VariablesMap&,
        const ExecutionContext& context)>; //!< Apply function signature

    /**
     * Create a memory object with the given runtime state. Returns the given task as is as a one-sized task list.
     *
     */
    Memory() noexcept;

    /**
     * Create a memory object with the given runtime state
     *
     * \param[in] apply      Override the apply function with a custom implementation
     */
    explicit Memory(ApplyFunction apply) noexcept;

    Memory(const Memory& other) = delete;
    Memory(Memory&& other) noexcept = delete;

    ~Memory() override = default;

    auto operator=(const Memory& other) = delete;
    auto operator=(Memory&& other) noexcept = delete;

    [[nodiscard]] auto getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const noexcept
        -> config::VariablesMap override;

    [[nodiscard]] auto apply(core::Task task,
                             const config::VariablesMap& variables,
                             const ExecutionContext& context) const
        -> core::Tasks override;

    [[nodiscard]] auto summary() const noexcept -> std::string override;

  private:
    ApplyFunction m_apply;
};
} // namespace execHelper::plugins

#endif /* __MEMORY_H__ */
