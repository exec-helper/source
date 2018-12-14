#ifndef PLUGIN_STRATEGY_INCLUDE
#define PLUGIN_STRATEGY_INCLUDE

#include <string>

#include "config/variablesMap.h"
#include "core/task.h"

#include "plugin.h"
#include "pluginUtils.h"

namespace execHelper {
namespace plugins {
namespace detail {
/**
 * \brief Helper for getting and applying configuration settings for the given T...
 */
template <typename... T> struct ApplySettings {
    /*! @copydoc ::execHelper::plugins::JobsLong::getVariables(config::VariablesMap&, const config::FleetingOptionsInterface&)
     */
    static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& options) noexcept {
        ApplySettings<T...>::getVariables(variables, options);
    }

    /*! @copydoc ::execHelper::plugins::JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    static void apply(core::Task& task,
                      const config::VariablesMap& variables) noexcept {
        ApplySettings<T...>::apply(task, variables);
    }
};

/**
 * \brief Helper for getting and applying configuration settings for the given T and Other...
 */
template <typename T, typename... Other> struct ApplySettings<T, Other...> {
    /*! @copydoc ::execHelper::plugins::JobsLong::getVariables(config::VariablesMap&, const config::FleetingOptionsInterface&)
     */
    static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& options) noexcept {
        T::getVariables(variables, options);
        ApplySettings<Other...>::getVariables(variables, options);
    }

    /*! @copydoc ::execHelper::plugins::JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    static void apply(core::Task& task,
                      const config::VariablesMap& variables) noexcept {
        T::apply(task, variables);
        ApplySettings<Other...>::apply(task, variables);
    }
};

/**
 * \brief Helper for getting and applying configuration settings for the given T
 */
template <typename T> struct ApplySettings<T> {
    /*! @copydoc ::execHelper::plugins::JobsLong::getVariables(config::VariablesMap&, const config::FleetingOptionsInterface&)
     */
    static void
    getVariables(config::VariablesMap& variables,
                 const config::FleetingOptionsInterface& options) noexcept {
        T::getVariables(variables, options);
    }

    /*! @copydoc ::execHelper::plugins::JobsLong::apply(core::Task&, const config::VariablesMap&)
     */
    static void apply(core::Task& task,
                      const config::VariablesMap& variables) noexcept {
        T::apply(task, variables);
    }
};
} // namespace detail

/**
 * Strategy pattern for creating plugins that define their configuration/parameter mapping at compile time.
 *
 * The pattern can simply be used as: `using PluginName = PluginStrategy<PluginDetails, ConfigurationMapping1, ConfigurationMapping2>;`
 * where PluginName can now be used as any other class.
 *   PluginDetails should be default constructable and implement the following interface:
 *      std::string getName() noexcept;
 *      std::string getBinary() noexcept;
 *
 *   All configuration mapping objects should be default constructable and implement the following interfaces:
 *      static void getVariables(config::VariablesMap&, const config::FleetingOptionsInterface&) noexcept;
 *      static void apply(core::Task& task, const config::VariablesMap& variables) noexcept;
 */
template <typename Binary, typename... Options>
class PluginStrategy : public Plugin {
  public:
    std::string getPluginName() const noexcept override {
        return m_binary.getName();
    }

    config::VariablesMap getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const
        noexcept override {
        config::VariablesMap variables(m_binary.getName());
        detail::ApplySettings<Options...>::getVariables(variables,
                                                        fleetingOptions);
        return variables;
    }

    bool apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept override {
        task.append(m_binary.getBinary());
        detail::ApplySettings<Options...>::apply(task, variables);

        for(const auto& combination : makePatternPermutator(patterns)) {
            core::Task newTask = replacePatternCombinations(task, combination);
            if(!registerTask(newTask)) {
                return false;
            }
        }
        return true;
    }

  private:
    Binary m_binary; ///< Contains the specific plugin details
};
} // namespace plugins
} // namespace execHelper

#endif /* PLUGIN_STRATEGY_INCLUDE */
