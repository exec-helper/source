#ifndef EXECUTION_CONTEXT_INCLUDE
#define EXECUTION_CONTEXT_INCLUDE

#include <map>
#include <memory>

#include "plugin.h"

namespace execHelper::config {
class FleetingOptionsInterface;
class PatternsHandler;
class SettingsNode;
} // namespace execHelper::config

namespace execHelper::plugins {
/**
 * @brief Class containing the current execution context
 *
 * Collection of references to the current execution context
 */
class ExecutionContext {
  public:
    /**
     * Create a new context
     *
     * \param[in] options   The fleeting options to use.
     * \param[in] settings  The settings to use
     * \param[in] patterns  The patterns to use
     * \param[in] plugins   The plugins to use
     *
     * \note The caller is responsible for making sure the lifetime of the given objects extend the lifetime of this object
     */
    ExecutionContext(const config::FleetingOptionsInterface& options,
                     const config::SettingsNode& settings,
                     const config::PatternsHandler& patterns,
                     const Plugins& plugins)
        : m_options(options),
          m_settings(settings),
          m_patterns(patterns),
          m_plugins(plugins) {
        ;
    }

    /**
     * Get the fleeting options for this context
     *
     * \returns The fleeting options for this context
     */
    [[nodiscard]] inline auto options() const noexcept
        -> const config::FleetingOptionsInterface& {
        return m_options;
    }

    /**
     * Get the settings for this context
     *
     * \returns The settings for this context
     */
    [[nodiscard]] inline auto settings() const noexcept
        -> const config::SettingsNode& {
        return m_settings;
    }

    /**
     * Get the patterns for this context
     *
     * \returns The patterns for this context
     */
    [[nodiscard]] inline auto patterns() const noexcept
        -> const config::PatternsHandler& {
        return m_patterns;
    }

    /**
     * Get the plugins for this context
     *
     * \returns The plugins for this context
     */
    [[nodiscard]] inline auto plugins() const noexcept -> const Plugins& {
        return m_plugins;
    }

  private:
    const config::FleetingOptionsInterface& m_options;
    const config::SettingsNode& m_settings;
    const config::PatternsHandler& m_patterns;
    const Plugins& m_plugins;
};
} // namespace execHelper::plugins

#endif /* EXECUTION_CONTEXT_INCLUDE */
